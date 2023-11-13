#include "Lexicon.h"

#include <unordered_map>

//=============================================================================
// Constructors and Destructors
//=============================================================================

Lexicon::Lexicon(Op op) : m_type(Type::OPERATOR), m_op(op) {}
Lexicon::Lexicon(double scalar) : m_type(Type::SCALAR), m_scalar(scalar) {}
Lexicon::Lexicon(Keyword kwd) : m_type(Type::KEYWORD), m_keyword(kwd) {}
Lexicon::Lexicon(::Type type) : m_type(Type::TYPE), m_vtype(type) {}
Lexicon::Lexicon(const std::string& ident) : m_type(Type::IDENTIFIER), m_stringdata(ident) {}

//=============================================================================
// Getters for union members
//=============================================================================

Lexicon::Type Lexicon::type() const {
    return m_type;
}

::Type Lexicon::vtype() const {
    return type() == Type::TYPE ? std::get<::Type>(m_vtype) : ::Type::NONETYPE;
}

double Lexicon::scalar() const {
    return std::get<double>(m_scalar);
}

Keyword Lexicon::keyword() const {
    return type() == Type::KEYWORD ? std::get<Keyword>(m_keyword) : Keyword::NONEKWD;
}

Op Lexicon::op() const {
    return type() == Type::OPERATOR ? std::get<Op>(m_op) : Op::NONE;
}

const std::string& Lexicon::ident() const {
    return std::get<std::string>(m_stringdata);
}

std::ostream& operator<<(std::ostream& os, const Lexicon& lex) {
    switch (lex.type()) {
        case Lexicon::Type::IDENTIFIER: return os << "[ IDENT: " << lex.ident() << " ]";
        case Lexicon::Type::SCALAR: return os << "[ SCALAR: " << lex.scalar() << " ]";
        case Lexicon::Type::OPERATOR: return os << "[ OP: " << lex.op() << " ]";
        case Lexicon::Type::KEYWORD: return os << "[ KEYWORD: " << lex.keyword() << " ]";
        case Lexicon::Type::TYPE: return os << "[ TYPE: " << lex.vtype() << " ]";
    }

    return os;
}

//=============================================================================
// Public Functions
//=============================================================================

static std::unordered_map<std::string, Op> operators = {
    { "+",  Op::ADD },
    { "-",  Op::SUB },
    { "*",  Op::MUL },
    { "**", Op::EXP },
    { "/",  Op::DIV },
    { "=",  Op::EQU },
    { "(",  Op::OPAREN },
    { ")",  Op::CPAREN },
    { "{",  Op::OSTMT },
    { "}",  Op::CSTMT },
    { ":",  Op::COLON },
    { ",",  Op::COMMA },
    { ";",  Op::SEMI },
    { "==", Op::BEQU },
    { "!=", Op::NEQU },
    { "<", Op::LT },
    { ">", Op::GT },
    { "<=", Op::LTE },
    { ">=", Op::GTE },
};

static std::string operator_chars = "+-*/=(){}:,;!<>";

static std::unordered_map<std::string, Keyword> keywords = {
    { "fn", Keyword::FN },
    { "let", Keyword::LET },
    { "const", Keyword::CONST },
    { "return", Keyword::RETURN },
    { "then", Keyword::THEN },
    { "pub", Keyword::PUB },
};

static std::unordered_map<std::string, Type> types = {
    { "void", Type::VOID },
    { "i8", Type::I8 },
    { "u8", Type::U8 },
    { "i16", Type::I16 },
    { "u16", Type::U16 },
    { "i32", Type::I32 },
    { "u32", Type::U32 },
    { "i64", Type::I64 },
    { "u64", Type::U64 },
    { "f32", Type::F32 },
    { "f64", Type::F64 },
};

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
    enum Working { NONE, IDENTIFIER, OPERATOR, NUMBER, COMMENT } current = Working::NONE;
    std::vector<Lexicon> lexes;
    std::string buffer;

    auto pushbuffer = [&]() {
        switch (current) {
            case Working::NONE: throw LexException("tried to push an empty buffer to lexes");
            case Working::IDENTIFIER:
                if (keywords.count(buffer)) {
                    lexes.push_back(Lexicon(keywords.at(buffer)));
                } else if (types.count(buffer)) {
                    lexes.push_back(Lexicon(types.at(buffer)));
                } else {
                    lexes.push_back(Lexicon(buffer));
                }
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
                else if (c == '#') { current = Working::COMMENT; break; }
                else throw LexException("invalid character");
                buffer.push_back(c);
            }
            break;
            case Working::IDENTIFIER: {
                // push buffer if whitespace
                if (isspace(c)) pushbuffer();
                else if (c == '#') {
                    pushbuffer();
                    current = Working::COMMENT;
                }
                else if (operator_chars.find(c) != std::string::npos) {
                    pushbuffer();
                    current = Working::OPERATOR;
                    buffer.push_back(c);
                } else if (isalnum(c)) {
                    buffer.push_back(c);
                } else {
                    throw LexException("invalid character in identifier");
                }
            }
            break;
            case Working::NUMBER: {
                if (isspace(c)) pushbuffer();
                else if (c == '#') {
                    pushbuffer();
                    current = Working::COMMENT;
                }
                else if (isdigit(c) || c == '.') {
                    buffer.push_back(c);
                } else if (isalpha(c)) {
                    throw LexException("expected a digit or '.'");
                } else if (operator_chars.find(c) != std::string::npos) {
                    pushbuffer();
                    current = Working::OPERATOR;
                    buffer.push_back(c);
                }
            }
            break;
            case Working::OPERATOR: {
                if (
                       buffer == "*" && c == '*'
                    || buffer == "=" && c == '='
                    || buffer == "!" && c == '='
                    || buffer == ">" && c == '='
                    || buffer == "<" && c == '='
                ) {
                    buffer.push_back(c);
                } else if (isspace(c)) {
                    pushbuffer();
                    current = Working::NONE;
                } else if (c == '#') {
                    pushbuffer();
                    current = Working::COMMENT;
                } else if (isalpha(c)) {
                    pushbuffer();
                    current = Working::IDENTIFIER;
                    buffer.push_back(c);
                } else if (isdigit(c) || c == '.') {
                    pushbuffer();
                    current = Working::NUMBER;
                    buffer.push_back(c);
                } else if (operator_chars.find(c) != std::string::npos) {
                    pushbuffer();
                    current = Working::OPERATOR;
                    buffer.push_back(c);
                } else {
                    throw LexException("not sure how this happened");
                }
            }
            break;
            case Working::COMMENT: {
                if (c == '\n') {
                    current = Working::NONE;
                }
            }
        }
    }

    if (!buffer.empty())
        pushbuffer();

    return lexes;
}
