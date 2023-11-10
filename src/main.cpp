#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Expression.h"
#include "Lexicon.h"

int main(int argc, char **argv) {
    std::unordered_map<std::string, Expression*> variables;

    for (;;) {
        std::string input;

        std::cout << "> ";
        getline(std::cin, input);

        if (input == ":q") break;

        std::vector<Lexicon> tokens;

        try {
            tokens = Lexicon::lex(input);
        } catch (LexException& e) {
            std::cerr << "error: tokenizer exception thrown: " << e.what() << std::endl;
        }

        Expression* expr;

        try {
            expr = Expression::parse(tokens);
        } catch (ParseException& e) {
            std::cerr << "error: parser exception thrown: " << e.what() << std::endl;
        }

        try {
            std::cout << expr->evaluate(variables) << std::endl;
        } catch (ParseException& e) {
            std::cerr << "error: runtime exception thrown: " << e.what() << std::endl;
        }
    }

    return 0;
}
