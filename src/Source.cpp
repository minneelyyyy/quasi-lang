#include "Source.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const Source& src) {
    size_t counter = 0;

    for (auto& function : src.functions) {
        os << "#" << counter++ << ": " << "fn " << function.name()
            << " " << function.return_type() << std::endl;
    }

    return os;
}

void Source::push(const Function& func) {
    // ignore macros for now
    if (func.name() == "") return;

    functions.push_back(func);
}

static Function parse_function(const std::vector<Lexicon>& lexes) {
    // first lexicon should always be fn
    Lexicon fn = lexes[0];

    auto get_end = [&]() -> size_t {
        size_t end = 0;

        for (auto& lex : lexes) {
            if (lex.op() == Op::OSTMT || lex.op() == Op::SEMI || lex.keyword() == Keyword::THEN) break;
            end++;
        }

        return end;
    };

    size_t size = get_end();

    std::string name;
    Type rettype = Type::VOID;

    if (fn.keyword() != Keyword::FN) {
        std::cerr << "no fn keyword found yet attempted to parse a function prototype" << std::endl;
        abort();
    }

    // proc lambda
    if (size == 1) {
        return FunctionPrototype("");
    }

    if (size == 2) {
        switch (lexes[1].type()) {
            // it's a named proc
            case Lexicon::Type::IDENTIFIER: return FunctionPrototype(lexes[1].ident());

            // it's a lambda function with a return type
            case Lexicon::Type::TYPE: return FunctionPrototype("", lexes[1].vtype());

            default:
                std::cerr << "fn expected a return type or identifier" << std::endl;
                abort();
        }
    }

    if (size == 3) {
        // handle only case of 3 where it's not `fn name type`, `fn ()`
        if (lexes[1].op() == Op::OPAREN && lexes[2].op() == Op::CPAREN) {
            return FunctionPrototype("");
        }

        name = lexes[1].ident();
        rettype = lexes[2].vtype();
    }

    // there has to be arguments of some sort
    if (size > 3) {
        // lambda function
        if (lexes[1].op() == Op::OPAREN) {

        }
    
        // function
        else if (lexes[1].type() == Lexicon::Type::IDENTIFIER) {
            name = lexes[1].ident();
        }

        // handle arguments for function
        if (lexes[2].op() == Op::OPAREN) {

        }

        // last element is always type in this case unlkess there is no type, in which case its a )
        if (lexes[size - 1].type() == Lexicon::Type::TYPE)
            rettype = lexes[size - 1].vtype();
    }

    return Function(name, rettype);
}

Source Source::parse(const std::vector<Lexicon>& lexes) {
    Source src;

    for (size_t i = 0; i < lexes.size(); i++) {
        if (lexes[i].keyword() == Keyword::FN) {
            Function func = parse_function(std::vector<Lexicon>(lexes.begin() + i, lexes.end()));

            std::vector<Lexicon> body;

            for (; i < lexes.size(); i++) {
                if (lexes[i].op() == Op::OSTMT || lexes[i].keyword() == Keyword::THEN || lexes[i].op() == Op::SEMI)
                    break;
            }

            if (lexes[i].op() == Op::OSTMT) {
                size_t depth = 1;
                i++;

                for (; i < lexes.size(); i++) {
                    if (lexes[i].op() == Op::OSTMT) depth++;
                    if (lexes[i].op() == Op::CSTMT) depth--;

                    if (depth == 0) break;

                    body.push_back(lexes[i]);
                }
            } else if (lexes[i].keyword() == Keyword::THEN) {
                i++;
                for (; i < lexes.size(); i++) {
                    body.push_back(lexes[i]);
                    if (lexes[i].op() == Op::SEMI) break;
                }
            }

            if (body.size()) func.attach_body(body);

            src.push(func);
        }
    }

    return src;
}
