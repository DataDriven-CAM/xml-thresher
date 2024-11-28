#include <cstdio>
#include <ranges>
#include <cmath>

//#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "graph/container/compressed_graph.hpp"

#include "io/xml/Path.h"

#include <iostream>
#include <locale>
#include <codecvt>

#include "antlr4-runtime.h"
#include "io/xpath/XPath31ParserBase.h"
#include "parsing/XPath31Lexer.h"
#include "parsing/XPath31Parser.h"


namespace sylvanmats::io::xml{

}

sylvanmats::io::xml::Path<std::u16string> operator"" _xp(const char16_t* c, size_t s){
    return sylvanmats::io::xml::Path<std::u16string>(c);
}
