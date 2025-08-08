#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>

#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <iterator>
#include <numbers>
#include <ranges>

#define protected public
#include "io/xml/Path.cpp"
#include "io/xml/Path.h"
#include "io/xml/Binder.h"
#include "io/xml/XMLLexer.h"
#include "io/xml/XMLParser.h"
#include "io/xpath/XPath31Lexer.h"
#include "io/xpath/XPath31Parser.h"

constexpr inline auto string_hash(const char *s) {
    unsigned long long hash{}, c{};
    for (auto p = s; *p; ++p, ++c) {
        hash += *p << c;
    }
    return hash;
}
 
constexpr inline auto operator""_sh(const char *s, size_t) {
    return string_hash(s);
}

TEST_SUITE("tests"){
    
TEST_CASE("test periodic table" * doctest::skip()) {
    try{
        sylvanmats::io::xml::Binder xmlThresher("/home/roger/NetBeansProjects/OOBackbone/data/periodic_table.xml", "/home/roger/NetBeansProjects/OOBackbone/schemas/periodic_table.xsd");
        std::cout<<"check "<<std::endl;
//        CHECK_EQ(xmlThresher.version, std::string("1.0"));
        CHECK_EQ(xmlThresher.encoding, std::string("UTF-8"));
//        CHECK_EQ(xmlThresher.schemaPrefix, u"pt");
        std::cout<<"make path "<<std::endl;
        sylvanmats::io::xml::Path<std::u16string>&& path2=u"/pt:elements/pt:element[@symbol='C']"_xp;
        xmlThresher(path2, [&xmlThresher](std::u16string_view& value){
            //std::cout<<" "<<xmlThresher.utf16conv.to_bytes(std::u16string(value.begin(),value.end()))<<std::endl;
        });
        //std::cout<<"end pt "<<std::endl;
    }
    catch(std::exception& ex){
    //std::cout<<"fail "<<ex.what()<<std::endl;
        FAIL(ex.what());
    }
}

TEST_CASE("test fragment xml"){

    std::ifstream file("../../cifio/db/oxygen_fragments.graphml");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        CHECK_EQ(content.size(),  1015);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16conv;
    std::u16string utf16 = utf16conv.from_bytes(content);
    sylvanmats::antlr4::xml::XMLParser xmlParser;
    xmlParser(utf16, [&utf16conv](sylvanmats::antlr4::xml::LG& ldagGraph, sylvanmats::antlr4::xml::PG& dagGraph){
        CHECK_EQ(graph::num_vertices(ldagGraph), 265);
        CHECK_EQ(graph::vertices(ldagGraph).size(), 265);
        CHECK_EQ(graph::num_edges(ldagGraph), 264);
        CHECK_EQ(graph::num_vertices(dagGraph), 112);
        CHECK_EQ(graph::vertices(dagGraph).size(), 112);
        CHECK_EQ(graph::num_edges(dagGraph), 111);
        for (auto&& oe : graph::edges(dagGraph, dagGraph[0])){
            graph::container::csr_row<unsigned int>& o=dagGraph[graph::target_id(dagGraph, oe)];
            auto& ov=graph::vertex_value(dagGraph, o);
            //std::cout<<ov.id<<" "<<ov.token<<" "<<ov.parser_token<<" "<<ov.mode<<std::endl;
            auto& lov=graph::vertex_value(ldagGraph, ldagGraph[ov.id]);
            //auto& lov=graph::vertex_value(ldagGraph, lo);
            std::u16string vvStr(lov.start, lov.stop);
            std::cout<<" "<<utf16conv.to_bytes(vvStr);
            if(lov.token==sylvanmats::antlr4::xml::LEXER_CLOSE || lov.token==sylvanmats::antlr4::xml::LEXER_SLASH_CLOSE || lov.token==sylvanmats::antlr4::xml::LEXER_SPECIAL_CLOSE)std::cout<<std::endl;
        }

    });

    std::string blockPath="/document/element/content/element\n";
    std::u16string ublockPath = utf16conv.from_bytes(blockPath);
    sylvanmats::antlr4::xpath::XPath31Parser xpath31Parser;
    xpath31Parser(ublockPath, [](sylvanmats::antlr4::xpath::LG& ldagGraph, sylvanmats::antlr4::xpath::PG& dagGraph){
        CHECK_EQ(graph::num_vertices(dagGraph), 3);
        CHECK_EQ(graph::vertices(dagGraph).size(), 3);
        CHECK_EQ(graph::num_edges(dagGraph), 2);
    });
        // antlr4::ANTLRInputStream input(content);
        // sylvanmats::XMLLexer lexer(&input);
        // antlr4::CommonTokenStream tokens(&lexer);

        // sylvanmats::XMLParser parser(&tokens);
        // parser.setBuildParseTree(true);
        // antlr4::tree::ParseTree* tree = parser.document();

        // const std::string blockPath="/document/element/content/element";
        // antlr4::tree::xpath::XPath xblockPath(&parser, blockPath);
        // std::vector<antlr4::tree::ParseTree*> dataBlock=xblockPath.evaluate(tree);
        // CHECK_EQ(dataBlock.size(), 3);
        // for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlock.begin();it!=dataBlock.end();it++){
        //     if (sylvanmats::XMLParser::ElementContext* r=dynamic_cast<sylvanmats::XMLParser::ElementContext*>((*it))) {
        //         if(r->content()!=nullptr){
        //             auto ei = r->content()->element();
        //             for(sylvanmats::XMLParser::ElementContext* node : ei | std::views::filter([&r](sylvanmats::XMLParser::ElementContext* di){ return !r->Name().empty() && r->Name(0)->getText().compare("graph")==0 && di->Name(0)->getText().compare("node")==0; })){
        //                 std::string id;
        //                 for(sylvanmats::XMLParser::AttributeContext* a : node->attribute()){

        //                     //std::cout<<"\t"<<a->Name()->getText()<<" "<<a->STRING()->getText()<<std::endl;
        //                     switch(string_hash(a->Name()->getText().c_str())){
        //                         case "id"_sh:
        //                             id=a->STRING()->getText();
        //                         break;
        //                     }
        //                 }
        //                 auto ed = node->content()->element();
        //                 for(sylvanmats::XMLParser::ElementContext* data : ed | std::views::filter([](sylvanmats::XMLParser::ElementContext* di){ return di->Name(0)->getText().compare("data")==0; })){
        //                     std::string name;
        //                     for(sylvanmats::XMLParser::AttributeContext* a : data->attribute()){

        //                         //std::cout<<"\t"<<a->Name()->getText()<<" "<<a->STRING()->getText()<<std::endl;
        //                         switch(string_hash(a->Name()->getText().c_str())){
        //                             case "key"_sh:
        //                                 name=a->STRING()->getText();
        //                             break;
        //                         }
        //                     }
        //                     //if(data->content()!=nullptr)std::cout<<data->content()->chardata(0)->TEXT()->getText()<<std::endl;
        //                 }
        //             }
        //             for(sylvanmats::XMLParser::ElementContext* e : ei | std::views::filter([&r](sylvanmats::XMLParser::ElementContext* di){ return !r->Name().empty() && r->Name(0)->getText().compare("graph")==0 && di->Name(0)->getText().compare("edge")==0; })){
        //                 for(sylvanmats::XMLParser::AttributeContext* a : e->attribute()){

        //                     //std::cout<<"\t"<<a->Name()->getText()<<" "<<a->STRING()->getText()<<std::endl;
        //                 }
        //             }
        //         }
        //     }
        // }


}

}