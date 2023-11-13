#pragma once

#include <string>
#include <optional>
#include "Lexicon.h"

class FunctionPrototype {
    std::string m_identifier;
    Type m_return_type;

public:
    FunctionPrototype(const std::string& ident, Type ret);
    FunctionPrototype(const std::string& ident);
    const std::string& name() const;
    Type return_type() const;
};

class Function {
    FunctionPrototype m_prototype;
    std::optional<std::vector<Lexicon>> m_body_lexes;

public:
    Function(const std::string& ident, Type ret);
    Function(const std::string& ident);
    Function(const FunctionPrototype& prototype);
    const std::string& name() const;
    Type return_type() const;
    void attach_body(const std::vector<Lexicon>& body);
};
