#pragma once

#include <exception>
#include <string>
#include <vector>
#include <variant>

#include "common.h"

class LexException : public std::exception {
    const char *m_message;

public:
    LexException(const char *msg) : m_message(msg) {}

    const char *what() { return m_message; }
};

class Lexicon {
public:
    Lexicon(Op op);
    Lexicon(double scalar);
    Lexicon(const std::string& ident);

    static std::vector<Lexicon> lex(const std::string& input);

    Type type() const;
    double scalar() const;
    Op op() const;
    const std::string& ident() const;

private:
    Type m_type;
    std::variant<double, Op, std::string> m_scalar, m_op, m_ident;
};