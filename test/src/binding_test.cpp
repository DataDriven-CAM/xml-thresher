/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#define protected public
#include "io/xml/Binder.h"
#include "io/tikz/GraphPublisher.h"

TEST_SUITE("bindings"){
    
TEST_CASE("test graphml schema")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/schemas/graphml/graphml.xsd", "");
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){});
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
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){});
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
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){});
    CHECK_EQ(graph::num_vertices(xmlReaper.dagGraph), 37);
    CHECK_EQ(graph::vertices(xmlReaper.dagGraph).size(), 37);
    CHECK_EQ(xmlReaper.vertices.size(), 37);
    CHECK_EQ(graph::num_edges(xmlReaper.dagGraph), 36);
    CHECK_EQ(xmlReaper.dag.size(), 36);
    //std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    //std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK(xmlReaper.schemaPrefix.empty());
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    /*for( const auto& n : xmlReaper.schemaComponentMap ) {
        std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }*/
    sylanmats::io::tikz::GraphPublisher graphPublisher;
    std::string&& tikzDrawing=graphPublisher(xmlReaper);
    std::filesystem::path filePath="../documents/xml_graph.tex";
    std::ofstream ofs(filePath);
    ofs<<tikzDrawing<<std::endl;
    ofs.close();
   
}

TEST_CASE("test 4hhb pdb xml")
{
        auto start = std::chrono::high_resolution_clock::now();
     sylvanmats::io::xml::Binder xmlReaper("/home/roger/Downloads/4hhb.xml", "/home/roger/Downloads/pdbx-v50.xsd");
    xmlReaper([](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){});
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "4hhb time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1.0e-9 << "s\n";
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK(!xmlReaper.schemaPrefix.empty());
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}

}
