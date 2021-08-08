
#include "../../headers/Symbolic/sWord.h"

using namespace mov;

Effects Effects::neutral{};

void sWord::definition_to_string() {
    println("[", name, "]");
    println("input parameters: ", effects.num_popped, " output frames: ", effects.num_pushed);
    for(auto &thing : instructions)
        print(thing->linked_word->name, " ");
    println();
}



BranchInstruction *Instruction::as_branch() {
    return (BranchInstruction*)(this);
}

BranchIfInstruction *Instruction::as_branchif() {
    return (BranchIfInstruction*)(this);
}

ReturnInstruction *Instruction::as_return() {
    return (ReturnInstruction*)(this);
}

bool Instruction::branchy(){
    return
    linked_word->id == BRANCH ||
    linked_word->id == BRANCHIF ||
    linked_word->id == EXIT;
}

std::string Instruction::to_string(){
    return linked_word->name;
}

ReturnInstruction::ReturnInstruction()
    : Instruction(new sWord("exit", EXIT), sData(nullptr)){}

std::string BranchInstruction::to_string() {
    return linked_word->name + "(" + std::to_string(jump_to->index) + ")";
}

std::string BranchIfInstruction::to_string() {
    return linked_word->name + "(" + std::to_string(jump_to_next->index) + ", " + std::to_string(jump_to_far->index) +")";
}
