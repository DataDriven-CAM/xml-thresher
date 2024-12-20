#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <typeinfo>

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"

#include "io/xml/Binder.h"
#include "graph/views/depth_first_search.hpp"
#include "graph/views/breadth_first_search.hpp"
#include "graph/views/vertexlist.hpp"

namespace sylanmats::io::tikz{

    class GraphPublisher{
    protected:
        std::string graphTemplate{};
    public:
        GraphPublisher(){
            std::string templateLocation=(getenv("JSONTHRESHER_DB_LOCATION")!=nullptr) ? std::string(getenv("JSONTHRESHER_DB_LOCATION"))+"/templates/tikz": "./templates/tikz";
            std::filesystem::path path=templateLocation+"/tikz.txt";
            std::ifstream file(path);
            graphTemplate=std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        };
        GraphPublisher(const GraphPublisher& orig) = delete;
        virtual ~GraphPublisher() = default;
    
        std::string operator ()(sylvanmats::io::xml::Binder& xmlBinder){
            std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
            std::string tree;
            std::vector<int> visited(graph::num_edges(xmlBinder.dagGraph), 0);
            auto it = std::ranges::find_if(graph::vertices(xmlBinder.dagGraph),
                                     [&](auto& u) { return graph::vertex_value(xmlBinder.dagGraph, u).index == 0; });
            graph::vertex_id_t<sylvanmats::io::xml::G> seed=static_cast<graph::vertex_id_t<sylvanmats::io::xml::G>>(it - begin(graph::vertices(xmlBinder.dagGraph)));
            auto dfs      = graph::views::sourced_edges_depth_first_search(xmlBinder.dagGraph, seed);
            size_t depth=dfs.depth();
            for (auto&& [uid,vid,uv] : dfs) {
                if (size(graph::edges(xmlBinder.dagGraph, uid))==0) {
                  continue;
                }
                visited[uid]++;
                auto uValue=graph::vertex_value(xmlBinder.dagGraph, *graph::find_vertex(xmlBinder.dagGraph, uid));
                if(depth==1)tree.append(std::to_string(uValue.index));
                depth=dfs.depth();
                if(depth==1)tree.append("{");
                  //auto ev=edge_value(xmlBinder.dagGraph, wv);
                  //std::cout<<typeid(wv).name()<<" "<<depth<<" "<<dfs.depth()<<" "<<size(graph::edges(xmlBinder.dagGraph, vid))<<" "<<visited[vid]<<" "<<size(graph::edges(xmlBinder.dagGraph, wid))<<" "<<visited[wid]<<std::endl;
                  //if (!visited[ev]) {
                  /*if(depth>dfs.depth()){
                    if(!tree.empty() && size(graph::edges(xmlBinder.dagGraph, vid))>1 && size(graph::edges(xmlBinder.dagGraph, vid))==visited[vid])tree.append("}");
                    if(!tree.empty())tree.append(";\n");
                    auto vValue=graph::vertex_value(xmlBinder.dagGraph, *graph::find_vertex(xmlBinder.dagGraph, vid));
                    tree.append(std::to_string(vValue.index));
                  }*/
                    auto& vValue=graph::vertex_value(xmlBinder.dagGraph, *graph::find_vertex(xmlBinder.dagGraph, vid));
//                std::cout<<" "<<uid<<" "<<cv.to_bytes(std::u16string(sylvanmats::io::xml::substr_view(xmlBinder.utf16, uValue.angle_start+1, uValue.angle_end)))<<" -> "<<vid<<" "<<cv.to_bytes(std::u16string(sylvanmats::io::xml::substr_view(xmlBinder.utf16, vValue.angle_start+1, vValue.angle_end)))<<" "<<depth<<std::endl;
                    tree.append(" -> ");
                    //if(depth<dfs.depth() && size(graph::edges(xmlBinder.dagGraph, vid))>1 && visited[vid]==0)tree.append("{");
                    tree.append("\"");
                    if(vValue.forward_slashA==0 && vValue.forward_slashB==0){
                        std::u16string pairKeyName(sylvanmats::io::xml::substr_view(xmlBinder.utf16, vValue.angle_start+1, vValue.angle_end));
                        size_t pos=std::u16string::npos;
                        while ((pos = pairKeyName.find(u"_")) != std::u16string::npos) {
                            pairKeyName.replace(pos, 1, u"-");
                        }
                        tree.append(cv.to_bytes(pairKeyName));
                    }
                    tree.append(std::to_string(vValue.index));
                    tree.append("\"");
                    if(size(graph::edges(xmlBinder.dagGraph, vid))==0)tree.append(" [mark]");
                    visited[vid]++;
                  //}
                    if(size(graph::edges(xmlBinder.dagGraph, vid))==0)tree.append("};\n");
            }
            auto cArg=fmt::arg("tree", tree);
            //auto dArg=fmt::arg("leafs", indices);
            std::string ret=fmt::vformat(graphTemplate, fmt::make_format_args(cArg));
            return ret;
        };
    };
}
