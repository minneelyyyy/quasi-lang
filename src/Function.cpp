#include "Function.h"

FunctionPrototype::FunctionPrototype(const std::string& ident) : m_identifier(ident), m_return_type(Type::VOID) {}
FunctionPrototype::FunctionPrototype(const std::string& ident, Type ret) : m_identifier(ident), m_return_type(ret) {}

const std::string& FunctionPrototype::name() const {
    return m_identifier;
}

Type FunctionPrototype::return_type() const {
    return m_return_type;
}

Function::Function(const std::string& ident) : m_prototype(FunctionPrototype(ident, Type::VOID)) {}
Function::Function(const std::string& ident, Type ret) : m_prototype(FunctionPrototype(ident, ret)) {}
Function::Function(const FunctionPrototype& prototype) : m_prototype(prototype) {}


const std::string& Function::name() const {
    return m_prototype.name();
}

Type Function::return_type() const {
    return m_prototype.return_type();
}

void Function::attach_body(const std::vector<Lexicon>& body) {
    m_body_lexes = body;
}
