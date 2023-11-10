#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <variant>
#include <optional>

enum Type { SCALAR, OPERATOR, IDENTIFIER };
enum Op { NONE, ADD, SUB, MUL, DIV, EXP, OPAREN, EQU, CPAREN };

class LexException : public std::exception {
    const char *m_message;

public:
    LexException(const char *msg) : m_message(msg) {}

    const char *what() { return m_message; }
};

class Lexicon {
    Type m_type;
    std::variant<double, Op, std::string> m_scalar, m_op, m_ident;

public:
    Lexicon(Op op) : m_type(Type::OPERATOR), m_op(op) {}
    Lexicon(double scalar) : m_type(Type::SCALAR), m_scalar(scalar) {}
    Lexicon(const std::string& ident) : m_type(Type::IDENTIFIER), m_ident(ident) {}

    Type type() const { return m_type; }
    double scalar() const { return std::get<double>(m_scalar); }
    Op op_unsafe() const { return std::get<Op>(m_op); }
    Op op() const noexcept {
        return type() == Type::OPERATOR ? op_unsafe() : Op::NONE;
    }
    const std::string& ident() const { return std::get<std::string>(m_ident); }

    static std::vector<Lexicon> lex(const std::string& input) {
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
                    else if (iswspace(c)) continue;
                    else throw LexException("invalid character");
                    buffer.push_back(c);
                }
                break;
                case Working::IDENTIFIER: {
                    // push buffer if whitespace
                    if (iswspace(c)) pushbuffer();

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
                    if (iswspace(c)) pushbuffer();

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
                    
                    else if (iswspace(c)) {
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
};

class ParseException : public std::exception {
    const char *m_message;

public:
    ParseException(const char *msg) : m_message(msg) {}

    const char *what() { return m_message; }
};

class Expression {
    Expression *left = nullptr, *right = nullptr;
    Type m_type;
    std::variant<double, Op, std::string> m_scalar, m_op, m_ident;

public:
    Expression() {}
    Expression(double scalar) : m_type(Type::SCALAR), m_scalar(scalar) {}
    Expression(Op op) : m_type(Type::OPERATOR), m_op(op) {}
    Expression(const std::string& ident) : m_type(Type::IDENTIFIER), m_ident(ident) {}

    Expression(const Lexicon& lex) {
        this->m_type = lex.type();
        switch (lex.type()) {
            case Type::SCALAR: this->m_scalar = lex.scalar();
            break;
            case Type::OPERATOR: this->m_op = lex.op();
            break;
            case Type::IDENTIFIER: this->m_ident = lex.ident();
            break;
        }
    }

    ~Expression() {
        if (left != nullptr) delete left;
        if (right != nullptr) delete right;
    }

    Type type() const { return m_type; }

    double scalar_unsafe() const { return std::get<double>(m_scalar); }
    std::optional<double> scalar() const noexcept {
        return type() == Type::SCALAR ? std::optional<double>(scalar_unsafe()) : std::nullopt;
    }

    Op op_unsafe() const { return std::get<Op>(m_op); }
    Op op() const noexcept {
        return type() == Type::OPERATOR ? op_unsafe() : Op::NONE;
    }

    const std::string& ident_unsafe() const { return std::get<std::string>(m_ident); }
    std::optional<std::string> ident() const noexcept {
        return type() == Type::IDENTIFIER ? std::optional<std::string>(ident_unsafe()) : std::nullopt;
    }

    double evaluate(std::unordered_map<std::string, Expression*>& variables) const {
        switch (op()) {
            case Op::ADD: {
                if (this->left == nullptr)
                    return +this->right->evaluate(variables);
                
                return this->left->evaluate(variables) + this->right->evaluate(variables);
            }
            case Op::SUB: {
                if (this->left == nullptr)
                    return -this->right->evaluate(variables);
                
                return this->left->evaluate(variables) - this->right->evaluate(variables);
            }
            case Op::MUL: return this->left->evaluate(variables) * this->right->evaluate(variables);
            case Op::DIV: return this->left->evaluate(variables) / this->right->evaluate(variables);
            case Op::EXP: return std::pow(this->left->evaluate(variables), this->right->evaluate(variables));
            case Op::EQU: {
                variables[this->left->ident_unsafe()] = this->right;
                return this->right->evaluate(variables);
            }
            case Op::OPAREN: return this->left->evaluate(variables);
        }

        switch (type()) {
            case Type::SCALAR: return scalar_unsafe();
            case Type::IDENTIFIER: return variables.at(ident_unsafe())->evaluate(variables);
        }

        throw ParseException("invalid parse tree");
    }

    int precedence() const {
        switch (op()) {
            case Op::OPAREN: return -5;
            case Op::EQU: return -4;
            case Op::ADD: case Op::SUB: return -3;
            case Op::MUL: case Op::DIV: return -2;
            case Op::EXP: return -1;
            default: return 0;
        }
    }

    static Expression* parse(const std::vector<Lexicon>& lex) {
        if (lex.size() == 0) throw ParseException("expected input");

        Expression* left = new Expression(lex[0]);
        size_t pos_op = 1;

        switch (left->op()) {
            case Op::OPAREN: {
                size_t depth = 1;

                for (int i = 1; i < lex.size() && depth > 0; i++, pos_op++) {
                    switch (lex[i].op()) {
                        case Op::OPAREN: depth++; break;
                        case Op::CPAREN: depth--; break;
                    }
                }

                if (depth != 0)
                    throw ParseException("expected a ')' to match");
                
                left->left = parse(std::vector<Lexicon>(lex.begin() + 1, lex.begin() + pos_op - 1));
            }
            break;
            case Op::SUB: case Op::ADD: {
                left->right = new Expression(lex[1]);
                pos_op += 1;
            }
            break;
            case Op::NONE: break;
            default: throw ParseException("unexpected operator, expected identifier or scalar");
        }

        if (pos_op >= lex.size())
            return left;

        Expression *current = new Expression(lex[pos_op]);

        if (current->type() != Type::OPERATOR) {
            throw ParseException("expected operator");
        }

        current->left = left;

        Expression* next = parse(std::vector<Lexicon>(lex.begin() + pos_op + 1, lex.end()));

        if (next->precedence() < current->precedence()) {
            current->right = next->left;
            next->left = current;
            current = next;
        } else {
            current->right = next;
        }

        return current;
    }
};

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
