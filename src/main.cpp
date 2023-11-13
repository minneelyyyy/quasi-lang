#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "Lexicon.h"
#include "Source.h"
#include "cxxopts.hpp"

int main(int argc, const char **argv) {
    cxxopts::Options options("quasi", "a computer language");

    options.add_options()
        ("v,verbose", "verbose compiler output", cxxopts::value<bool>()->default_value("false"))
        ;
    
    options.allow_unrecognised_options();

    auto result = options.parse(argc, argv);

    bool verbose;

    if (verbose = result["verbose"].as<bool>()) {
        std::cout << "verbose output is enabled" << std::endl;
    }

    for (auto& f : result.unmatched()) {
        std::ifstream t(f);
        std::stringstream stream;
        stream << t.rdbuf();

        if (verbose)
            std::cout << "Compiling " << f << std::endl;

        if (verbose)
            std::cout << "tokenizing..." << std::endl;

        std::vector<Lexicon> lexes = Lexicon::lex(stream.str());

        if (verbose)
            std::cout << "parsing..." << std::endl;

        Source src = Source::parse(lexes);

        if (verbose) {
            std::cout << "Functions: " << std::endl;
            std::cout << src << std::endl;
        }
    }

    return 0;
}
