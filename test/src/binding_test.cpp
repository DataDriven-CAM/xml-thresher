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
#include "io/tikz/GraphPublisher.h"

#include "io/xml/Path.h"
#include "io/xml/Binder.h"
#include "graph/algorithm/connected_components.hpp"

TEST_SUITE("bindings"){

TEST_CASE("test note.xml"){
    sylvanmats::io::xml::Binder xmlReaper("./examples/note.xml", "");
    xmlReaper([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
    CHECK_EQ(xmlReaper.version, std::string(""));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK(xmlReaper.schemaPrefix.empty());
    CHECK_EQ(graph::num_vertices(xmlReaper.dagGraph), 10);
    CHECK_EQ(graph::vertices(xmlReaper.dagGraph).size(), 10);
    CHECK_EQ(xmlReaper.vertices.size(), 10);
    CHECK_EQ(graph::num_edges(xmlReaper.dagGraph), 9);
    sylvanmats::io::xml::Path<std::u16string> path(u"/note/body");
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    xmlReaper(path, [&cv](std::u16string_view& value){
        std::cout<<" "<<cv.to_bytes(std::u16string(value.begin(), value.end()))<<std::endl;
        CHECK_EQ(value, u"My yoke is easy and my burden is light.");
    });
    sylvanmats::io::xml::Path<std::u16string> dPath(u"/note/from[@description]");
    xmlReaper(dPath, [&cv](std::u16string_view& value){
        std::cout<<" "<<cv.to_bytes(std::u16string(value.begin(), value.end()))<<std::endl;
        CHECK_EQ(value, u"The Way");
    });
}

TEST_CASE("test graphml schema")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/schemas/graphml/graphml.xsd", "");
    xmlReaper([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK_EQ(xmlReaper.schemaPrefix, u"xs");
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        //std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}

TEST_CASE("test graphml schema angle bracket hunt")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/schemas/graphml/graphml.xsd", "");
    xmlReaper([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK_EQ(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()), u"xs");
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        //std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}

TEST_CASE("test graphml xml")
{
    sylvanmats::io::xml::Binder xmlReaper("../../cifio/db/oxygen_fragments.graphml", "");
    xmlReaper([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
    CHECK_EQ(graph::num_vertices(xmlReaper.dagGraph), 37);
    CHECK_EQ(graph::vertices(xmlReaper.dagGraph).size(), 37);
    CHECK_EQ(xmlReaper.vertices.size(), 37);
    CHECK_EQ(graph::num_edges(xmlReaper.dagGraph), 36);
    //std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    //std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK(xmlReaper.schemaPrefix.empty());
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    /*for( const auto& n : xmlReaper.schemaComponentMap ) {
        std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }*/
    for(std::vector<std::vector<size_t>>::iterator it=xmlReaper.depthProfile.begin();it!=xmlReaper.depthProfile.end();it++){
        std::cout<<(std::distance(xmlReaper.depthProfile.begin(), it))<<std::endl;
        std::string depthProfile=fmt::format("{}\n", (*it));
        std::cout<<"\t"<<depthProfile;
    }
    std::vector<size_t> component(graph::num_vertices((xmlReaper.dagGraph)));
    graph::connected_components(xmlReaper.dagGraph, component);
    CHECK_EQ(component.size(), 37);
    std::unordered_map<size_t, size_t> islandMap;
    for(std::vector<size_t>::iterator it=component.begin();it!=component.end();it++){
        //std::cout<<" "<<(*it);
        if(islandMap.count((*it))>0)islandMap[(*it)]++;
        else islandMap[(*it)]=1;
    }
    std::cout<<std::endl;
    CHECK_EQ(islandMap.size(), 1);
    if(islandMap.size()>0)CHECK_EQ(islandMap[0], 37);
    sylanmats::io::tikz::GraphPublisher graphPublisher;
    std::string&& tikzDrawing=graphPublisher(xmlReaper);
    std::filesystem::path filePath="../documents/xml_graph.tex";
    std::ofstream ofs(filePath);
    ofs<<tikzDrawing<<std::endl;
    ofs.close();
   
}

TEST_CASE("test 4hhb pdb xml" * doctest::skip())
{
        auto start = std::chrono::high_resolution_clock::now();
     sylvanmats::io::xml::Binder xmlReaper("/home/roger/Downloads/4hhb.xml", "/home/roger/Downloads/pdbx-v50.xsd");
    xmlReaper([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "4hhb time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()*1.0e-9 << "s\n";
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    std::cout<<xmlReaper.version<<" "<<xmlReaper.encoding<<" "<<cv.to_bytes(std::u16string(xmlReaper.schemaPrefix.begin(), xmlReaper.schemaPrefix.end()))<<std::endl;
    CHECK_EQ(xmlReaper.version, std::string("1.0"));
    CHECK_EQ(xmlReaper.encoding, std::string("UTF-8"));
    CHECK(!xmlReaper.schemaPrefix.empty());
    //for(size_t i=0;i<xmlReaper.schemaComponentMap.size();i++)std::cout<<cv.to_bytes(std::u16string(xmlReaper.schemaComponentMap.begin(), xmlReaper.schemaComponentMap.end()))<<std::endl;
    for( const auto& n : xmlReaper.schemaComponentMap ) {
        //std::cout << "Key:[" << cv.to_bytes(n.first) << "] Value:[" << cv.to_bytes(std::u16string(n.second.begin(), n.second.end())) << "]\n";
    }
   
}

}
