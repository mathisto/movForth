

#ifndef MOVFORTH_IDATA_H
#define MOVFORTH_IDATA_H

#include <variant>
#include <string>
#include "../Print.h"

namespace mov {
    class iWord;
    typedef iWord *iWordptr;

    struct iData : std::variant<std::nullptr_t, int, iWordptr>{
        using var_type = std::variant<std::nullptr_t, int, iWordptr>;

        explicit iData(var_type data) : var_type(data) {}
        iData() : var_type(nullptr) {}

        bool is_number(){
            return index() == 1;
        }
        bool is_word(){
            return index() == 2;
        }
        bool is_empty(){
            return index() == 0;
        }

        int as_number(){
            try {
                return std::get<int>(*this);
            }catch (...){
                println("tried extract variant of type " , index() , " as num");
                return 0;
            }
        }
        iWordptr as_word(){
            try {
                return std::get<iWordptr>(*this);
            }catch (...){
                println("tried extract variant of type " , index() , " as word");
                return nullptr;
            }
        }

        std::string to_string();
        iData clone();
    };
}
#endif //MOVFORTH_IDATA_H
