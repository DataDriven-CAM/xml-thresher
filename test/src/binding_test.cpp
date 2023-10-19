/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#define protected public
#include "io/xml/Binder.h"

TEST_SUITE("bindings"){
TEST_CASE("test graphml schema")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/schemas/graphml/graphml.xsd", "");
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>, std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>>>>& dag){});
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK_EQ(xmlReaper.schemaPrefix, u"xs");
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}

TEST_CASE("test graphml schema angle bracket hunt")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/schemas/graphml/graphml.xsd", "");
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>, std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>>>>& dag){});
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK_EQ(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()), u"xs");
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}

TEST_CASE("test graphml xml")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/db/oxygen_fragments.graphml", "");
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>, std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t>>>>& dag){});
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK(xmlReaper.schemaPrefix.empty());
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}
}
