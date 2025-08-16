#pragma once

#include "io/xpath/XPath31Lexer.h"

namespace sylvanmats::antlr4::xpath {
    class XPath31Parser;

    class XPath31ParserBase  {
        private:
        std::vector<ast_node> vertices;
    public:
        XPath31ParserBase() = default;
        XPath31ParserBase(std::vector<ast_node>& vertices) : vertices (vertices) {};
        XPath31ParserBase(const XPath31ParserBase& orig) = delete;
        virtual ~XPath31ParserBase() = default;

        bool IsFuncCall();

    };
}
