
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/Pass.h"
//#include "headers/Generation/IRGenerator.h"

int main() {
    //hello_word();

    // Create a plain old interpreter that interprets
    // the contents of the Forth file boot.fs
    mov::Interpreter interpreter("../boot.fs");

    // Extract the compiled word "test" from the interpreter's
    // dictionary. "test" has pointers to each of its definition words,
    // which also have pointers to each of their definition words
    auto word_to_compile = interpreter.find("test");

    // Create an analysis object for static analysis of
    // the compiled definition
    mov::Analysis analysis;

    // Do static analysis on "test".
    // 1. Control Analysis (compute the basic blocks of
    //    a definition and look for stack size inconsistencies
    //    between basic block edges)
    // 2. Data Flow Analysis (graph of data exchange between words.
    //    Stack elements are nodes and registers are edges. This is
    //    how the compiler converts from stack to SSA format)
    auto converted_word = analysis.static_analysis(word_to_compile);

    // Show the fruits of labor from static analysis
    println();
    println();
    mov::Analysis::show_word_info(converted_word);
}

