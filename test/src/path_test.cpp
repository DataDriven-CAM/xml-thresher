
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <cstdio>
#include <doctest/doctest.h>
#include <iostream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#include <concepts>
#include <fstream>
#include <regex>
#include <filesystem>

#define protected public

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

#include "io/xpath/XPath31Lexer.h"
#include "io/xpath/XPath31Parser.h"

TEST_SUITE("paths"){
TEST_CASE("test path as container")
{
//    static_assert(std::copy_constructible<sylvanmats::io::xml::Path<std::u16string>>);
//    static_assert(std::input_or_output_iterator<sylvanmats::io::xml::Path<std::u16string>>::iterator);
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    sylvanmats::io::xml::Path<std::u16string> path(u"/xsd:schema/xsd:element[@name]");
    CHECK_EQ(path.size(), 2);
    sylvanmats::io::xml::Path<std::u16string>&& path2=u"/xsd:schema/xsd:element[@name]"_xp;
    CHECK_EQ(path2.size(), 2);
    for(sylvanmats::io::xml::Path<std::u16string>::iterator it=path2.begin();it!=path2.end();it++){
        //std::cout<<" "<<cv.to_bytes(*it)<<std::endl;
    }
}

TEST_CASE("test antlrv4 xpath parsing"){
    std::string content="/pt:elements/pt:element[@symbol='C']";
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf16 = utf16conv.from_bytes(content);
    /*sylvanmats::antlr4::parse::XPath31Parser xpath31Parser;
    xpath31Parser(utf16, [](sylvanmats::antlr4::parse::LG& ldagGraph, sylvanmats::antlr4::parse::PG& dagGraph){
        CHECK_EQ(graph::num_vertices(dagGraph), 18);
        CHECK_EQ(graph::vertices(dagGraph).size(), 18);
        CHECK_EQ(graph::num_edges(dagGraph), 17);
    });*/
 }

TEST_CASE("test xpath expressions"){
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::filesystem::path expressionsPath="/home/roger/NodeProjects/grammars-v4/xpath/xpath31/examples/expressions.txt";
    std::ifstream file(expressionsPath.string().c_str(), std::ios::in | std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::regex lines_regex("[^;^\n]+");
    for (std::sregex_iterator i = std::sregex_iterator(content.begin(), content.end(), lines_regex); i != std::sregex_iterator(); ++i){
        std::smatch match = *i;
        std::string match_str = match.str();
        //std::cout << match_str << '\n';
        try{
            sylvanmats::io::xml::Path<std::u16string> path(cv.from_bytes(match_str).c_str());
        }
        catch(std::range_error ex){
            std::cout<<" "<<ex.what()<<std::endl;
        }
    }
}

}