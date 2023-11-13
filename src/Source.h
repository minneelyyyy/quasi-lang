#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "Lexicon.h"
#include "Function.h"

// parse an entire file
class Source {
    std::vector<Function> functions;
public:
    void push(const Function& func);
    static Source parse(const std::vector<Lexicon>& lex);
    friend std::ostream& operator<<(std::ostream& os, const Source& src);
};
