#include <cstdio>
#include <ranges>
#include <cmath>

#include "graph/container/compressed_graph.hpp"

#include "io/xml/Path.h"

#include <iostream>
#include <locale>
#include <codecvt>

namespace sylvanmats::io::xml{

}

sylvanmats::io::xml::Path<std::u16string> operator""_xp(const char16_t* c, size_t s){
    return sylvanmats::io::xml::Path<std::u16string>(c);
}
