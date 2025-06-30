#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#define protected public
#include "io/tikz/GraphPublisher.h"

#include "io/xml/Path.h"
#include "io/xslt/Morpher.h"
#include "graph/algorithm/connected_components.hpp"

TEST_SUITE("morphings"){

TEST_CASE("test note.xml to list"){
    sylvanmats::io::xslt::Morpher xmlMorpher("./examples/note.xml", "./examples/note2list.xsl");
    xmlMorpher([](std::u16string& utf16){});
}

TEST_CASE("test transforming 4hhb"){
    sylvanmats::io::xslt::Morpher xmlMorpher("/home/roger/Downloads/4hhb.xml", "./examples/pdbx2cif.xsl");
    xmlMorpher([](std::u16string& utf16){});
}

}
