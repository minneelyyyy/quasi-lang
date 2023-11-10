#include "Lexicon.h"

#include <unordered_map>

//=============================================================================
// Constructors and Destructors
//=============================================================================

Lexicon::Lexicon(Op op) : m_type(Type::OPERATOR), m_op(op) {}
Lexicon::Lexicon(double scalar) : m_type(Type::SCALAR), m_scalar(scalar) {}
Lexicon::Lexicon(const std::string& ident) : m_type(Type::IDENTIFIER), m_ident(ident) {}

//=============================================================================
// Getters for union members
//=============================================================================

Type Lexicon::type() const {
    return m_type;
}

double Lexicon::scalar() const {
    return std::get<double>(m_scalar);
}

Op Lexicon::op() const {
    return type() == Type::OPERATOR ? std::get<Op>(m_op) : Op::NONE;
}

const std::string& Lexicon::ident() const {
    return std::get<std::string>(m_ident);
}

//=============================================================================
// Public Functions
//=============================================================================

static std::unordered_map<std::string, Op> operators = {
    { "+", Op::ADD },
    { "-", Op::SUB },
    { "*", Op::MUL },
    { "/", Op::DIV },
    { "**", Op::EXP },
    { "(", Op::OPAREN },
    { ")", Op::CPAREN },
    { "=", Op::EQU },
};

static std::string operator_chars = "+-*/()=";

#include <iostream>

// TODO: make not bad
static bool is_valid_number(const std::string& str) {
    bool already_hit_decimal = false;

    if (str.front() == '.' && str.back() == '.') return false;

    for (char c : str) {
        if (c == '.')
            if (already_hit_decimal) return false;
            else already_hit_decimal = true;
    }

    return true;
}

std::vector<Lexicon> Lexicon::lex(const std::string& input) {
    enum Working { NONE, IDENTIFIER, OPERATOR, NUMBER } current = Working::NONE;
    std::vector<Lexicon> lexes;
    std::string buffer;

    auto pushbuffer = [&]() {
        switch (current) {
            case Working::NONE: throw LexException("tried to push an empty buffer to lexes");
            case Working::IDENTIFIER:
                lexes.push_back(Lexicon(buffer));
            break;
            case Working::NUMBER:
                if (!is_valid_number(buffer))
                    throw LexException("invalid number");

                lexes.push_back(Lexicon(std::stod(buffer)));
            break;
            case Working::OPERATOR: lexes.push_back(Lexicon(operators.at(buffer)));
            break;
        }

        buffer.clear();
        current = Working::NONE;
    };

    for (char c : input) {
        switch (current) {
            case Working::NONE: {
                if (isalpha(c)) current = Working::IDENTIFIER;
                else if (isalnum(c) || c == '.') current = Working::NUMBER;
                else if (operator_chars.find(c) != std::string::npos) current = Working::OPERATOR;
                else if (isspace(c)) continue;
                else throw LexException("invalid character");
                buffer.push_back(c);
            }
            break;
            case Working::IDENTIFIER: {
                // push buffer if whitespace
                if (isspace(c)) pushbuffer();

                // push the identifier if an operator character and then continue as operator
                else if (operator_chars.find(c) != std::string::npos) {
                    pushbuffer();
                    current = Working::OPERATOR;
                    buffer.push_back(c);
                }

                // just push the character if its alnum
                else if (isalnum(c)) {
                    buffer.push_back(c);
                }

                // none matched
                else {
                    throw LexException("invalid character in identifier");
                }
            }
            break;
            case Working::NUMBER: {
                // push buffer if whitespace
                if (isspace(c)) pushbuffer();

                // push digits or .
                else if (isdigit(c) || c == '.') {
                    buffer.push_back(c);
                }

                // error if identifier
                else if (isalpha(c)) {
                    throw LexException("expected a digit or '.'");
                }

                // go again if operator
                else if (operator_chars.find(c) != std::string::npos) {
                    pushbuffer();
                    current = Working::OPERATOR;
                    buffer.push_back(c);
                }
            }
            break;
            case Working::OPERATOR: {
                if (buffer == "*" && c == '*') {
                    buffer.push_back(c);
                }
                
                else if (isspace(c)) {
                    pushbuffer();
                }
                
                else if (isalpha(c)) {
                    pushbuffer();
                    current = Working::IDENTIFIER;
                    buffer.push_back(c);
                }

                else if (isdigit(c) || c == '.') {
                    pushbuffer();
                    current = Working::NUMBER;
                    buffer.push_back(c);
                }

                else if (operator_chars.find(c) != std::string::npos) {
                    pushbuffer();
                    current = Working::OPERATOR;
                    buffer.push_back(c);
                }

                else {
                    throw LexException("not sure how this happened");
                }
            }
        }
    }

    if (!buffer.empty())
        pushbuffer();

    return lexes;
}