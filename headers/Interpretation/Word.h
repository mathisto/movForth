#ifndef MOVFORTH_INTER_WORD_H
#define MOVFORTH_INTER_WORD_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include <string>

namespace mfc{
    class Stack;
    class Word;
    typedef Word *Wordptr;

    struct Data : std::variant<std::nullptr_t, int, Wordptr>{
        using var_type = std::variant<std::nullptr_t, int, Wordptr>;

        explicit Data(var_type data) : var_type(data) {}
        Data() : var_type(nullptr) {}

        bool is_num() { return index() == 1; }
        bool is_xt() { return index() == 2; }
        bool is_undef() { return index() == 0; }

        int as_num() { return std::get<int>(*this); }
        Wordptr as_xt() { return std::get<Wordptr>(*this); }

        std::string to_string();
        Data clone();
    };

    class IP{
        const bool isActive;
    public:
        std::vector<Wordptr>::iterator me;

        explicit IP(std::vector<Wordptr>::iterator in) : me(in), isActive(true) {}
        IP() : isActive(false) {}

        IP operator+=(int i) { //outside the class
            if(!isActive) return *this;
            me += i;
            return *this;
        }

        bool operator<(const std::vector<Wordptr>::iterator& a) const{
            return me < a;
        }
    };


    class Word{
        const std::string name;
    public:
        bool immediate;
        const bool stateful;
        Data data = Data(); // has value if stateful

        Word(std::string name, bool immediate, bool stateful);

        virtual void execute(Stack &stack, IP &ip) = 0;
        virtual Wordptr clone() = 0;

        virtual std::string to_string();
        virtual std::string base_string();
    };

    template <typename Cast>
    static Cast* try_cast(Wordptr word_pointer){
        return dynamic_cast<Cast*>(word_pointer);
    }

    class ForthWord : public Word {
        std::vector<Wordptr> definition;
    public:
        ForthWord(std::string name, bool immediate);

        void execute(Stack &stack, IP &ip) override;
        Wordptr clone() override;
        void add(Data data);
        void set(int index, Data value);
        void definition_to_string();
        int definition_size();
        std::vector<Wordptr> get_definition() {
            return definition;
        }
    };

    class Primitive : public Word{
        std::function<void(Stack&, Data data, IP&)> action;
    public:
        Primitive(std::string name, bool immediate, std::function<void(Stack&, Data data, IP&)> action, bool stateful);
        void execute(Stack &stack, IP &ip) override;
        Wordptr clone() override;
    };
}

#endif
