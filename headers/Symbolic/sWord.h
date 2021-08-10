
#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

#include <utility>
#include "Structures.h"
#include "../PrimitiveEnums.h"
#include "Effects.h"
#include "BasicBlock.h"

namespace mov{

    class sWord{
    public:
        primitive_words id = primitive_words::OTHER;
        const std::string name;

        explicit sWord(std::string name, primitive_words id);
        explicit sWord(std::string name, primitive_words id, Effects effects);

        std::vector<BasicBlock> basic_blocks{};

        Effects effects;

        bool branchy();
    };

}

#endif //MOVFORTH_WORD_H
