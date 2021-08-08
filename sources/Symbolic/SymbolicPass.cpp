
#include <vector>
#include <set>
#include <stack>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Print.h"

using namespace mov;

sData StackGrapher::symbolize_data(iData data) {
    if (data.is_number())
        return sData(data.as_number());
    if (data.is_word())
        return sData(compute_effects(data.as_word()));
    if(data.is_empty())
        return sData(nullptr);
    println("FUCK");
    return sData(nullptr);
}

sWordptr StackGrapher::compute_effects(iWordptr original_word) {
    // check to see if we have passed over this word already
    // if so, return a pointer to it
    auto cached = visited_words.find(original_word);
    if (cached != visited_words.end())
        return cached->second;

    if (dynamic_cast<Primitive *>(original_word))
    {   // is a primitive: return the singleton of the primitive
        sWordptr word_singleton = primitive_lookup.at(original_word->id);
        return word_singleton;
    }

    if (auto forth_word = dynamic_cast<ForthWord *>(original_word))
    {
        dln();
        dln("compute [", original_word->name(), "]");
        indent();

        auto converted = translate_to_basic_blocks(forth_word);
        stack_graph_pass(converted);
        retrieve_push_pop_effects(converted);

        unindent();
        dln("finished compute [", original_word->name(), "]");

        return converted;
    }

    dln("word is null");

    return nullptr;
}

sWordptr StackGrapher::show_word_info(sWordptr wordptr) {
    println("============[", wordptr->name, "]===========");
    println("Basic blocks:");

    for(const auto& bbe : wordptr->basic_blocks){
        println("bbe " + std::to_string(bbe.index) + ":");
        print("    ");
        for(auto instr : bbe.instructions)
            print(" " , instr->name());
        println();
    }
    println();

    return wordptr;
}

