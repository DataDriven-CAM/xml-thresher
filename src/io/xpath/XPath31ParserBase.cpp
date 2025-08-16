

#include "io/xpath/XPath31ParserBase.h"

#include "io/xpath/XPath31Parser.h"

namespace sylvanmats::antlr4::xpath {

    bool XPath31ParserBase::IsFuncCall(){
        return !(
            std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_ARRAY;})!= vertices.end()
            || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_ATTRIBUTE;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_COMMENT;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_DOCUMENT_NODE;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_ELEMENT;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_EMPTY_SEQUENCE;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_FUNCTION;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_IF;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_ITEM;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_MAP;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_NAMESPACE_NODE;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_NODE;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_PROCESSING_INSTRUCTION;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_SCHEMA_ATTRIBUTE;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_SCHEMA_ELEMENT;})!= vertices.end()
                || std::find_if(vertices.begin(), vertices.end(), [](const ast_node& p) {return p.token_start == LEXER_TOKEN::LEXER_KW_TEXT;})!= vertices.end()
        );
    };

}
