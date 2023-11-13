#pragma once

#include <exception>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "Lexicon.h"

class ParseException : public std::exception {
    const char *m_message;

public:
    ParseException(const char *msg) : m_message(msg) {}

    const char *what() { return m_message; }
};

// represents any expression
class Expression {
public:
    Expression();
    Expression(double scalar);
    Expression(Op op);
    Expression(const std::string& ident);
    Expression(const Lexicon& lex);
    ~Expression();

    double evaluate(std::unordered_map<std::string, double>& variables) const;
    static Expression* parse(const std::vector<Lexicon>& lex);
    int precedence() const;

private:
    Expression *left = nullptr, *right = nullptr;
    Lexicon::Type m_type;
    std::variant<double, Op, std::string> m_scalar, m_op, m_ident;

private:
    double scalar() const;
    Op op() const;
    const std::string& ident() const;
};
