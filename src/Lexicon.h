#pragma once

#include <exception>
#include <string>
#include <vector>
#include <variant>
#include <fstream>

enum Type {
    NONETYPE,
    I8, U8, I16, U16, I32, U32, I64, U64,
    F32, F64,
    VOID,
};

static std::ostream& operator<<(std::ostream& os, Type t) {
    switch (t) {
        case Type::VOID: return os << "void";
        case Type::I8: return os << "i8";
        case Type::U8: return os << "u8";
        case Type::I16: return os << "i16";
        case Type::U16: return os << "u16";
        case Type::I32: return os << "i32";
        case Type::U32: return os << "u32";
        case Type::I64: return os << "i64";
        case Type::U64: return os << "u64";
        case Type::F32: return os << "f32";
        case Type::F64: return os << "f64";
    }

    return os;
}

enum Op {
    NONE,
    ADD,
    SUB,
    MUL,
    EXP,
    DIV,
    OPAREN,
    CPAREN,
    OSTMT,
    CSTMT,
    COLON,
    COMMA,
    SEMI,
    EQU,
    BEQU,
    NEQU,
    NOT,
    LT,
    GT,
    LTE,
    GTE,
};

static std::ostream& operator<<(std::ostream& os, Op op) {
    switch (op) {
        case Op::NONE: return os << "NONE";
        case Op::ADD: return os << "+";
        case Op::SUB: return os << "-";
        case Op::MUL: return os << "*";
        case Op::EXP: return os << "**";
        case Op::DIV: return os << "/";
        case Op::OPAREN: return os << "(";
        case Op::CPAREN: return os << ")";
        case Op::OSTMT: return os << "{";
        case Op::CSTMT: return os << "}";
        case Op::COLON: return os << ":";
        case Op::COMMA: return os << ",";
        case Op::SEMI: return os << ";";
        case Op::EQU: return os << "=";
        case Op::BEQU: return os << "==";
        case Op::NEQU: return os << "!=";
        case Op::NOT: return os << "!";
        case Op::LT: return os << "<";
        case Op::GT: return os << ">";
        case Op::LTE: return os << "<=";
        case Op::GTE: return os << ">=";
    }

    return os;
}

enum Keyword {
    NONEKWD,
    FN,
    LET,
    CONST,
    RETURN,
    THEN,
    PUB,
};

static std::ostream& operator<<(std::ostream& os, Keyword kwd) {
    switch (kwd) {
        case Keyword::FN: return os << "fn";
        case Keyword::LET: return os << "let";
        case Keyword::CONST: return os << "const";
        case Keyword::RETURN: return os << "return";
        case Keyword::THEN: return os << "then";
        case Keyword::PUB: return os << "pub";
    }

    return os;
}

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
    Lexicon(Keyword kwd);
    Lexicon(::Type type);
    Lexicon(const std::string& ident);

    static std::vector<Lexicon> lex(const std::string& input);

    enum Type { SCALAR, OPERATOR, IDENTIFIER, TYPE, KEYWORD };
    Type type() const;
    ::Type vtype() const;
    double scalar() const;
    Op op() const;
    Keyword keyword() const;
    const std::string& ident() const;

private:
    Type m_type;
    std::variant<double, Op, Keyword, ::Type, std::string> m_scalar, m_op, m_keyword, m_vtype, m_stringdata;
    friend std::ostream& operator<<(std::ostream& os, const Lexicon& lex);
};
