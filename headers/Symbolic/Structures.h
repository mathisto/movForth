
#ifndef MOVFORTH_SYM_TYPES_H
#define MOVFORTH_SYM_TYPES_H

#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <functional>
#include "../Util.h"
#include "../Print.h"

/*
 * So many graphs! Graphs, graphs, everywhere!
 * Crisscrossed pointers and recursive structures,
 * multiple operations and general D I S S O N A N C E
 *
 * tl;dr it was a lot easier to use raw pointers than
 * to deal with references and shared_ptr
 */

namespace sym {

    class Word;

    typedef Word *Wordptr;

    class Data {
        std::variant<int, Wordptr, std::nullptr_t> data{};
    public:
        bool is_num() {return data.index() == 0;}
        bool is_xt() {return data.index() == 1;}
        bool is_unknown() {return data.index() == 2;};

        int as_num() {return std::get<int>(data);}
        Wordptr as_xt() {return std::get<Wordptr>(data);}

        explicit Data(std::variant<int, Wordptr, std::nullptr_t> data) : data(data) {}
    };

    struct Register{
        unsigned int ID = -1;
        enum registerType {UNDEF, NORMAL, PARAM} register_type = UNDEF;

        Register operator++(int val){
            ID++;
            return *this;
        }

        std::string to_string(){
            switch (register_type)
            {
                case NORMAL:
                    return "(register "+ std::to_string(ID) + ")";
                case PARAM:
                    return "(input "+ std::to_string(ID) + ")";
                case UNDEF:
                    return "(undefined "+ std::to_string(ID) + ")";
                default:
                    return "(fucked "+ std::to_string(ID) + ")";
            }
        }
    };

    struct RegisterGenerator{
        Register get(){
            auto ret = current_id;
            current_id++;
            return ret;
        }
        Register get_param(){
            auto ret = current_param_id;
            current_param_id++;
            return ret;
        }

    private:
        Register current_id = {0, Register::NORMAL};
        Register current_param_id = {0, Register::PARAM};
    };

    struct Node{
        Node* target = nullptr;
        Register edge_register;

        static void link(Node* back, Node* front, Register id){
            front->target = back;
            front->edge_register = id;
        }
    };

    // thin wrapper for convenience
    struct NodeList : std::vector<Node*>{
        Node* push_back(Node* push){
            std::vector<Node*>::push_back(push);
            return back();
        }

        Node* pop_back(){
            auto top = back();
            std::vector<Node*>::pop_back();
            return top;
        }

        Node* penultimate(){
            return operator[](size() - 2);
        }

        unsigned int size(){
            return (unsigned int) std::vector<Node*>::size();
        }

        static void move_top_elements(NodeList &from, NodeList &to, int num){
            to.insert(to.end(), std::make_move_iterator(from.end() - num),
                      std::make_move_iterator(from.end()));
            from.erase(from.end() - num, from.end());
        }
    };
}

#endif
