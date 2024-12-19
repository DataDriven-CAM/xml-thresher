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
            auto vlist = graph::views::vertexlist(xmlBinder.dagGraph);
            for (auto&& [uid, u] : graph::views::vertexlist(xmlBinder.dagGraph)) {
                if (visited[uid]>0 || size(graph::edges(xmlBinder.dagGraph, uid))==0) {
                  continue;
                }
                visited[uid]++;
                auto uValue=graph::vertex_value(xmlBinder.dagGraph, *graph::find_vertex(xmlBinder.dagGraph, uid));
                tree.append(std::to_string(uValue.index));
                if(size(graph::edges(xmlBinder.dagGraph, uid))>1)tree.append("{");
                auto dfs=graph::views::sourced_edges_depth_first_search(xmlBinder.dagGraph, uid);
                size_t depth=dfs.depth();
                size_t notchDepth=depth;
                for (auto&& [vid, wid, wv] : dfs) {
                  //auto ev=edge_value(xmlBinder.dagGraph, wv);
                  //std::cout<<typeid(wv).name()<<" "<<depth<<" "<<dfs.depth()<<" "<<size(graph::edges(xmlBinder.dagGraph, vid))<<" "<<visited[vid]<<" "<<size(graph::edges(xmlBinder.dagGraph, wid))<<" "<<visited[wid]<<std::endl;
                  //if (!visited[ev]) {
                  if(depth>dfs.depth()){
                    if(!tree.empty() && size(graph::edges(xmlBinder.dagGraph, vid))>1 && size(graph::edges(xmlBinder.dagGraph, vid))==visited[vid])tree.append("}");
                    if(!tree.empty())tree.append(";\n");
                    auto vValue=graph::vertex_value(xmlBinder.dagGraph, *graph::find_vertex(xmlBinder.dagGraph, vid));
                    tree.append(std::to_string(vValue.index));
                  }
                    auto& wValue=graph::vertex_value(xmlBinder.dagGraph, *graph::find_vertex(xmlBinder.dagGraph, wid));
                    tree.append(" -> ");
                    if(depth<dfs.depth() && size(graph::edges(xmlBinder.dagGraph, vid))>1 && visited[vid]==0)tree.append("{");
                    tree.append("\"");
                    if(wValue.forward_slashA==0 && wValue.forward_slashB==0){
                        std::u16string pairKeyName(sylvanmats::io::xml::substr_view(xmlBinder.utf16, wValue.angle_start+1, wValue.angle_end));
                        size_t pos=std::u16string::npos;
                        while ((pos = pairKeyName.find(u"_")) != std::u16string::npos) {
                            pairKeyName.replace(pos, 1, u"-");
                        }
                        tree.append(cv.to_bytes(pairKeyName));
                    }
                    tree.append(std::to_string(wValue.index));
                    tree.append("\"");
                    if(size(graph::edges(xmlBinder.dagGraph, wid))==0)tree.append(" [mark]");
                    visited[vid]++;
                  //}
                  if(depth>dfs.depth())notchDepth=dfs.depth();
                  depth=dfs.depth();
                }
                    tree.append("};\n");
            }
            auto cArg=fmt::arg("tree", tree);
            //auto dArg=fmt::arg("leafs", indices);
            std::string ret=fmt::vformat(graphTemplate, fmt::make_format_args(cArg));
            return ret;
        };
    };
}
