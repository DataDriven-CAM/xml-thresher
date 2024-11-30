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

#include "antlr4-runtime.h"
#include "parsing/XMLLexer.h"
#include "parsing/XMLParser.h"

constexpr inline auto string_hash(const char *s) {
    unsigned long long hash{}, c{};
    for (auto p = s; *p; ++p, ++c) {
        hash += *p << c;
    }
    return hash;
}
 
constexpr inline auto operator"" _sh(const char *s, size_t) {
    return string_hash(s);
}

TEST_SUITE("tests"){
    
TEST_CASE("test periodic table") {
    try{
        sylvanmats::io::xml::Binder xmlThresher("/home/roger/NetBeansProjects/OOBackbone/data/periodic_table.xml", "/home/roger/NetBeansProjects/OOBackbone/schemas/periodic_table.xsd");
        std::cout<<"check "<<std::endl;
//        CHECK_EQ(xmlThresher.version, std::string("1.0"));
        CHECK_EQ(xmlThresher.encoding, std::string("UTF-8"));
//        CHECK_EQ(xmlThresher.schemaPrefix, u"pt");
        std::cout<<"make path "<<std::endl;
        sylvanmats::io::xml::Path<std::u16string>&& path2=u"/pt:elements/pt:element[@symbol='C']"_xp;
        xmlThresher(path2, [&xmlThresher](std::u16string_view& value){
            std::cout<<" "<<xmlThresher.utf16conv.to_bytes(std::u16string(value.begin(),value.end()))<<std::endl;
        });
        std::cout<<"end pt "<<std::endl;
    }
    catch(std::exception& ex){
    std::cout<<"fail "<<ex.what()<<std::endl;
        FAIL(ex.what());
    }
        std::cout<<"end2 pt "<<std::endl;
}

TEST_CASE("test fragment xml"){

    std::ifstream file("../../cifio/db/oxygen_fragments.graphml");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        CHECK_EQ(content.size(), 1016);
        antlr4::ANTLRInputStream input(content);
        sylvanmats::XMLLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);

        sylvanmats::XMLParser parser(&tokens);
        parser.setBuildParseTree(true);
        antlr4::tree::ParseTree* tree = parser.document();

        const std::string blockPath="/document/element/content/element";
        antlr4::tree::xpath::XPath xblockPath(&parser, blockPath);
        std::vector<antlr4::tree::ParseTree*> dataBlock=xblockPath.evaluate(tree);
        CHECK_EQ(dataBlock.size(), 3);
        for(std::vector<antlr4::tree::ParseTree*>::iterator it=dataBlock.begin();it!=dataBlock.end();it++){
            if (sylvanmats::XMLParser::ElementContext* r=dynamic_cast<sylvanmats::XMLParser::ElementContext*>((*it))) {
                if(r->content()!=nullptr){
                    auto ei = r->content()->element();
                    for(sylvanmats::XMLParser::ElementContext* node : ei | std::views::filter([&r](sylvanmats::XMLParser::ElementContext* di){ return !r->Name().empty() && r->Name(0)->getText().compare("graph")==0 && di->Name(0)->getText().compare("node")==0; })){
                        std::string id;
                        for(sylvanmats::XMLParser::AttributeContext* a : node->attribute()){

                            std::cout<<"\t"<<a->Name()->getText()<<" "<<a->STRING()->getText()<<std::endl;
                            switch(string_hash(a->Name()->getText().c_str())){
                                case "id"_sh:
                                    id=a->STRING()->getText();
                                break;
                            }
                        }
                        auto ed = node->content()->element();
                        for(sylvanmats::XMLParser::ElementContext* data : ed | std::views::filter([](sylvanmats::XMLParser::ElementContext* di){ return di->Name(0)->getText().compare("data")==0; })){
                            std::string name;
                            for(sylvanmats::XMLParser::AttributeContext* a : data->attribute()){

                                std::cout<<"\t"<<a->Name()->getText()<<" "<<a->STRING()->getText()<<std::endl;
                                switch(string_hash(a->Name()->getText().c_str())){
                                    case "key"_sh:
                                        name=a->STRING()->getText();
                                    break;
                                }
                            }
                            if(data->content()!=nullptr)std::cout<<data->content()->chardata(0)->TEXT()->getText()<<std::endl;
                        }
                    }
                    for(sylvanmats::XMLParser::ElementContext* e : ei | std::views::filter([&r](sylvanmats::XMLParser::ElementContext* di){ return !r->Name().empty() && r->Name(0)->getText().compare("graph")==0 && di->Name(0)->getText().compare("edge")==0; })){
                        for(sylvanmats::XMLParser::AttributeContext* a : e->attribute()){

                            std::cout<<"\t"<<a->Name()->getText()<<" "<<a->STRING()->getText()<<std::endl;
                        }
                    }
                }
            }
        }
}

}