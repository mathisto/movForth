
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
        println("find id " , original_word->id, " " , original_word->base_string());
        sWordptr word_singleton = primitive_lookup.at(original_word->id);
        return word_singleton;
    }

    if (auto forth_word = dynamic_cast<ForthWord *>(original_word))
    {
        dln();
        dln("compute [", original_word->base_string(), "]");
        indent();

        auto converted = translate_to_basic_blocks(forth_word);
        stack_graph_pass(converted);
        retrieve_push_pop_effects(converted);

        unindent();
        dln("finished compute [", original_word->base_string(), "]");

        return converted;
    }

    dln("word is null");

    return nullptr;
}

void propagate_stack(NodeList &stack, Instruction *instruction, sWordptr base,
                     RegisterGenerator &register_generator) {
    auto effects = instruction->linked_word->effects;

    dln("pops: ", effects.num_popped, " pushes: ", effects.num_pushed);

    // add necessary input nodes
    unsigned int nodes_from_input = 0;
    unsigned int nodes_from_stack = effects.num_popped;
    if (effects.num_popped > stack.size())
    {
        nodes_from_input = effects.num_popped - stack.size();
        nodes_from_stack = stack.size();
    }
    while (nodes_from_input-- > 0)
    {
        Register input_register = register_generator.get_param();
        auto input_node = new Node;
        instruction->pop_nodes.push_back(new Node{
                .target = input_node,
                .edge_register = input_register});
        base->my_graphs_inputs.push_front(input_node);
        dln("needs extra input ", input_register.to_string());
    }

    // pop input nodes from stack
    NodeList::move_top_elements(stack, instruction->pop_nodes, (int) nodes_from_stack);

    // make empty output nodes
    for (int i = 0; i < effects.num_pushed; i++)
        instruction->push_nodes.push_back(new Node);

    // cross internally
    for (auto out_in_pair : effects.out_in_pairs)
    {
        // dln("cross internal o", out_in_pair.second, " with ",out_in_pair.first);

        auto pop_node = instruction->pop_nodes[out_in_pair.second];
        auto push_node = instruction->push_nodes[out_in_pair.first];

        Node::link(pop_node, push_node, pop_node->edge_register);
    }

    // push output nodes to stack
    for (auto push_node : instruction->push_nodes)
    {
        Register aRegister;
        if (push_node->target != nullptr) // it was linked in the [cross internally] step
            aRegister = push_node->edge_register;
        else
            aRegister = register_generator.get();

        stack.push_back(new Node);
        Node::link_bidirection(push_node, stack.back(), aRegister);
    }

    println("pop from ids:");
    for (auto node : instruction->pop_nodes)
        println("   ", node->edge_register.to_string());
    dln("push to ids");
    for (auto thing : instruction->push_nodes)
        dln("   ", thing->forward_edge_register.to_string());
}

void StackGrapher::stack_graph_pass(sWordptr word) {
    // the stack is a constantly updated list of
    // loose pop nodes that the next word might need
    auto &running_stack = *(new NodeList);
    RegisterGenerator register_generator;

    for (auto instruction : word->instructions)
    {
        auto definee = instruction->linked_word;
        if(definee->name == "nop")
            continue;

        // update the word's total Effects
        word->effects.acquire_side_effects(definee->effects);

        // propagate the stack state
        dln();
        dln("[", definee->name, "]");
        propagate_stack(running_stack, instruction, word, register_generator);

        dln();
        dln("intermediate stack: ");
        for (auto thing : running_stack)
        {
            dln("\t", thing->edge_register.to_string());
        }
    }

    // the pushed side effect is the same as remaining stack frames
    word->effects.num_pushed = (int) running_stack.size();
    word->my_graphs_outputs = running_stack;
}

void StackGrapher::retrieve_push_pop_effects(sWordptr word) {
    // matching pairs
    for (int i = 0; i < word->my_graphs_outputs.size(); i++)
    {
        Node *output = word->my_graphs_outputs[i];

        Node *next = output->target;
        while (true)
        {
            if (next->target == nullptr)
                break;
            if (next->edge_register.register_type == Register::PARAM)
            {
                word->effects.out_in_pairs.emplace_back(i, next->edge_register.ID);
                break;
            }
            next = next->target;
        }
    }

    // push pop effects
    dln();
    dln("Finished effects for ", word->name, " pops: ",
        word->my_graphs_inputs.size(), " pushes: ",
        word->my_graphs_outputs.size());
    word->effects.num_pushed = (int) word->my_graphs_outputs.size();
    word->effects.num_popped = (int) word->my_graphs_inputs.size();
}

sWordptr StackGrapher::show_word_info(sWordptr wordptr) {
    println("============[", wordptr->name, "]===========");
    println("Basic blocks:");

    for(const auto& bbe : wordptr->basic_blocks){
        println("bbe " + std::to_string(bbe.index) + ":");
        print("    ");
        for(auto instr : bbe.instructions)
            print(" " , instr->to_string());
        println();
    }
    println();

    return wordptr;
}

