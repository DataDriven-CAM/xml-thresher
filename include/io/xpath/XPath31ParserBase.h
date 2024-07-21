#pragma once

#include "antlr4-runtime.h"

namespace sylvanmats {
    class XPath31Parser;

    class XPath31ParserBase : public antlr4::Parser {
    public:
        XPath31ParserBase() = delete;
        XPath31ParserBase(antlr4::TokenStream* input) : antlr4::Parser(input) {};
        XPath31ParserBase(const XPath31ParserBase& orig) = delete;
        virtual ~XPath31ParserBase() = default;

        bool IsFuncCall();

    };
}
