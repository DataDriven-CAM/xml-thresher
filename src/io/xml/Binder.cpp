/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Binder.cpp
 * Author: roger
 * 
 * Created on February 22, 2020, 7:58 AM
 */

#include <cstdio>
#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "graph/container/compressed_graph.hpp"

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

#include <ranges>

namespace std{
    size_t u16ncmp(const char16_t* a, const char16_t* b, size_t n){
        for(size_t i=0;i<n;i++){
            if(a[i]>b[i])return 1;
            else if(a[i]<b[i])return -1;
        }
        return 0;
    }

}

namespace sylvanmats::io::xml{

Binder::Binder(std::string xmlPath, std::string xsdPath) : loc (std::locale("en_US.UTF-8"), new std::codecvt_utf8<char16_t>) {
    if(!xsdPath.empty()){
        try {
            const int fdXsd = open(xsdPath.c_str(), O_RDONLY);
            mio::mmap_source mmapXsd(fdXsd, 0, mio::map_entire_file);
            mio::mmap_source::const_iterator itg=mmapXsd.begin();
            if(!findProlog(itg))itg=mmapXsd.begin();
            else{
                xsdUTF16 = utf16conv.from_bytes(&mmapXsd[std::distance(mmapXsd.begin(), itg)]);
                initializeGraph(xsdUTF16, xsdDepthList, xsdDAG, xsdSchemaComponentMap);
//            std::string depthText=fmt::format("{}\n", xsdDepthList);
//            std::cout<<depthText;
    sylvanmats::io::xml::Path<std::u16string>&& path=u"/xsd:schema/xsd:element"_xp;
    std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>::iterator itDAG=xsdDAG.begin();
    sylvanmats::io::xml::Path<std::u16string>::iterator it=path.begin();
    if(itDAG!=xsdDAG.end() && (xsdUTF16.substr((*itDAG).first.angle_start+1, (*itDAG).first.space-(*itDAG).first.angle_start-1).compare((*it))==0)){
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cv;
//        std::cout<<"index "<<(*itDAG).first.index<<" "<<cv.to_bytes(xsdUTF16.substr((*itDAG).first.angle_start+1, (*itDAG).first.space-(*itDAG).first.angle_start-1))<<std::endl;
//        std::cout<<"angle_start "<<(*itDAG).first.angle_start<<std::endl;
//        std::cout<<"space "<<(*itDAG).first.space<<std::endl;
//        std::cout<<"angle_end "<<(*itDAG).first.angle_end<<std::endl;
        ++it;
        std::cout<<"match begin "<<" "<<(*itDAG).second.size()<<" "<<cv.to_bytes((*it))<<std::endl;
        for(auto itS : (*itDAG).second | std::views::filter([&](sylvanmats::io::xml::tag_indexer& di){ return xsdUTF16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<"+(*it)); })){
        std::cout<<"index "<<itS.index<<" "<<cv.to_bytes(xsdUTF16.substr(itS.angle_start+1, itS.angle_end-itS.angle_start-1))<<std::endl;
            
        }
        
    }
                
            }
            mmapXsd.unmap();
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    try {
        const int fdXml = open(xmlPath.c_str(), O_RDONLY);
        mio::mmap_source mmapXml(fdXml, 0, mio::map_entire_file);
        mio::mmap_source::const_iterator itg=mmapXml.begin();
        if(!findProlog(itg))itg=mmapXml.begin();
        {
            utf16 = utf16conv.from_bytes(&(*itg));
            initializeGraph(utf16, depthList, dag, schemaComponentMap);
        }
        mmapXml.unmap();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
}

    void Binder::operator()(std::function<void(std::u16string& utf16, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag)> apply){
        apply(utf16, dag);
    }
    
    void Binder::operator()(sylvanmats::io::xml::Path<std::u16string>& xp, std::function<void(std::u16string_view& value)> apply){
            bool hit=xp.match(*this, [](sylvanmats::io::xml::Mask& mask){
                std::cout<<"matched "<<std::endl;
            });
            std::cout<<"binder get "<<hit<<std::endl;
            /*bool hit=match(xp, true, [&](size_t index, std::u16string_view key, std::any& v){
                std::u16string_view u16v=std::any_cast<std::u16string_view>(v);
                apply(u16v);
                return true;
            });*/
    }
    
    std::u16string Binder::findAttribute(std::u16string name, size_t start, size_t end){
        std::u16string attr=name+u"=\"";
        std::u16string::size_type offset=utf16.substr(start, end-start).find(attr);
        if(offset!=std::u16string::npos){
            offset=start+offset+attr.size();
            std::u16string::size_type endOffset=offset;
            while(endOffset<end && utf16.at(endOffset)!=u'"'){
                endOffset++;
            }
            if(endOffset<end)return utf16.substr(offset, endOffset-offset);
        }
        return u"";
    }
    
    void Binder::initializeGraph(std::u16string& utf16, std::vector<int>& depthList, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag, std::unordered_map<std::u16string, std::u16string_view>& schemaComponentMap){

            std::u16string::const_iterator it = utf16.cbegin();
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cv;
            int count=0;
            bool commentsOnly=true;
            while(commentsOnly && it!=utf16.end()){
                skipOverWhiteSpaces(it);
                if(commentStart(it)){std::advance(it, 4);skipOverComment(it);}
                else commentsOnly=false;
            }
            depth=0;
            for(int di=0;di<nDepth;di++)depthProfile.push_back(std::vector<size_t>{});
            findSchemaComponents(utf16, it, depthList, dag, schemaComponentMap);
            skipOverWhiteSpaces(it);
            std::u16string::const_iterator temp=it;
            size_t dagOffset=(!dag.empty())? dag.size()-1 : 0;
            while(temp!=utf16.end()){
                if(commentStart(temp)){std::advance(temp, 4);skipOverComment(temp);}
                if((*temp)==u'<'){
                    size_t angleStart=std::distance(utf16.cbegin(), temp);
                    std::u16string::const_iterator forwardSlashIt=temp;
                    size_t forwardSlash=0;
                    while((*temp)!=u'>' && (*temp)!=u'/' && (*temp)!=u' '){if(temp==utf16.cend())break;++temp;}
                    if((*temp)==u'/' && std::distance(forwardSlashIt, temp)==1){
                        forwardSlash=1;
                        while((*temp)!=u'>' && (*temp)!=u' '){if(temp==utf16.cend())break;++temp;}
                    }
                    size_t angleSpace=std::distance(utf16.cbegin(), temp);
                    size_t forwardSlash2=0;
                    if((*temp)==u' '){
                        while((*temp)!=u'>' && (*temp)!=u'/'){if(temp==utf16.cend())break;++temp;}
                        if((*temp)==u'/'){
                            forwardSlash2=1;
                            while((*temp)!=u'>'){if(temp==utf16.cend())break;++temp;}
                        }
                    }
                    if((*temp)==u'>'){
                        size_t angleEnd=std::distance(utf16.cbegin(), temp);
                        if(forwardSlash2==0){
                        vertices.push_back(tag_indexer{.index=vertices.size(), .angle_start=angleStart, .forward_slashA=forwardSlash, .space=angleSpace, .forward_slashB=forwardSlash2, .angle_end=angleEnd, .depth=nDepth});
                        if(nDepth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
                        depthProfile[nDepth].push_back(vertices.back().index);
                        }
                        dag.push_back(std::make_pair(tag_indexer{.index=dag.size(), .angle_start=angleStart, .forward_slashA=forwardSlash, .space=angleSpace, .forward_slashB=forwardSlash2, .angle_end=angleEnd}, std::vector<tag_indexer>{}));
                        if(forwardSlash==1){
                            //std::cout<<"\t"<<cv.to_bytes(std::u16string(utf16.cbegin()+vertices.back().angle_start, utf16.cbegin()+vertices.back().angle_end+1))<<std::endl;
                            depthList.push_back(depth);
                            if(depth>0)depth--;
                            if(nDepth>0)nDepth--;
                        }
                        else if(forwardSlash2==1){
                            depth++;
                            depthList.push_back(depth);
                            if(depth>0)depth--;
                            //if(nDepth>0)nDepth--;
                        }
                        else {
                            //std::cout<<""<<cv.to_bytes(std::u16string(utf16.cbegin()+vertices.back().angle_start, utf16.cbegin()+vertices.back().angle_end+1))<<std::endl;
                            depth++;
                            nDepth++;
                            depthList.push_back(depth);
                        }
                        ++temp;
                    }
//                    std::u16string::const_iterator temper=temp;
//                    std::cout<<"angle bracket? "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<" "<<cv.to_bytes(*temper++)<<std::endl;
                }
                else ++temp;
            }
            /*std::string depthText=fmt::format("{}\n", depthList);
            std::cout<<depthText;
            for(std::vector<std::vector<size_t>>::iterator it=depthProfile.begin();it!=depthProfile.end();it++){
                std::cout<<(std::distance(depthProfile.begin(), it))<<std::endl;
                std::string depthProfile=fmt::format("{}\n", (*it));
                std::cout<<"\t"<<depthProfile;
            }*/
            for(std::vector<tag_indexer>::iterator itDag=vertices.begin()+dagOffset;itDag!=vertices.end();itDag++){
                size_t currentDepth=(*itDag).depth;
                if((*itDag).forward_slashA==0 && (*itDag).forward_slashB==0){
                    if(currentDepth>0){
                        //size_t parentObjSize=depthProfile[currentDepth-1].back();
                        bool hit=false;
                        size_t parentObjSize=bisect(currentDepth-1, (*itDag).index, hit);
                        //for(std::vector<size_t>::reverse_iterator it=depthProfile[currentDepth-1].rbegin();!hit && it!=depthProfile[currentDepth-1].rend();it++)
                        //    if(parentObjSize>=(*itDag).index)parentObjSize=vertices[(*it)].index;
                        //    else hit=true;
                        //if(vertices[parentObjSize].index<(*itDag).index)
                        if(hit)edges.push_back(std::make_tuple(vertices[parentObjSize].index, (*itDag).index, 1));
                    }
                }
                else if((*itDag).forward_slashA==1 && (*itDag).forward_slashB==0){
                    //if(currentDepth==0)std::cout<<"END_OBJ "<<(*itDag).index<<" "<<std::endl;
                    if(currentDepth>0){
                        //size_t parentObjSize=depthProfile[currentDepth].back();
                        bool hit=false;
                        size_t parentObjSize=bisect(currentDepth-1, 0, (*itDag).index, hit);
                        //for(std::vector<size_t>::reverse_iterator it=depthProfile[currentDepth].rbegin();!hit && it!=depthProfile[currentDepth].rend();it++)
                        //    if(parentObjSize>=(*itDag).index || vertices[parentObjSize].forward_slashA==0)parentObjSize=vertices[(*it)].index;
                        //    else hit=true;
                        //if(vertices[parentObjSize].index<(*itDag).index)
                        if(hit)edges.push_back(std::make_tuple(vertices[parentObjSize].index, (*itDag).index, 1));
                    }
                }
                else if((*itDag).forward_slashA==0 && (*itDag).forward_slashB==1){
                    if(currentDepth>=0){
                        //size_t parentObjSize=depthProfile[currentDepth-1].back();
                        bool hit=false;
                        size_t parentObjSize=bisect(currentDepth, (*itDag).index, hit);
                        //size_t offset=((*itDag).forward_slashB==1) ? 1: 0;
                        //for(std::vector<size_t>::reverse_iterator it=depthProfile[currentDepth-offset].rbegin();!hit && it!=depthProfile[currentDepth-offset].rend();it++)
                        //if(parentObjSize>=(*itDag).index)parentObjSize=vertices[(*it)].index;
                        //else hit=true;
//                        std::cout<<"VP "<<hit<<" "<<offset<<" "<<(currentDepth-offset)<<" "<<parentObjSize<<" "<<vertices[parentObjSize].index<<" "<<(*itDag).index<<std::endl;
                        //if(vertices[parentObjSize].index<(*itDag).index)
                        if(hit)edges.push_back(std::make_tuple(vertices[parentObjSize].index, (*itDag).index, 1));
                    }
                }
            }
            std::sort(edges.begin(), edges.end(), [](std::tuple<graph::vertex_id_t<G>, graph::vertex_id_t<G>, int>& a, std::tuple<graph::vertex_id_t<G>, graph::vertex_id_t<G>, int>& b){return std::get<0>(a)<std::get<0>(b) || std::get<1>(a)<std::get<1>(b);});
            /*std::cout<<"{";
            for(std::vector<std::tuple<graph::vertex_id_t<G>, graph::vertex_id_t<G>, int>>::iterator it=edges.begin();it!=edges.end();it++){
                std::cout<<"{"<<std::get<0>((*it))<<","<<std::get<1>((*it))<<","<<std::get<2>((*it))<<"}, ";
            }
            std::cout<<"}"<<std::endl;*/
            depth=0;
            std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>::iterator previousNode=dag.end();
            for(std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>::iterator itDag=dag.begin();itDag!=dag.end();itDag++){
                size_t currentDepth=depth;
                if((*itDag).first.forward_slashA==0 && (*itDag).first.forward_slashB==0){
                    if(previousNode!=dag.end()){
                        (*previousNode).second.push_back((*itDag).first);
                        (*itDag).second.push_back((*previousNode).first);
                    }
                    previousNode=itDag;
                    depth++;
                }
                else if((*itDag).first.forward_slashA==1 && (*itDag).first.forward_slashB==0){
                    if(previousNode!=dag.end()){
                        size_t currentOffset=(*itDag).first.index;
                        size_t vOffset=(currentOffset>0) ? currentOffset-1: 0;
                        int currentDepth=(depthList[currentOffset]>0 && currentOffset<depthList.size()-1 && depthList[currentOffset+1]==depthList[currentOffset]) ? depthList[currentOffset]-1: depthList[currentOffset];
                        while(vOffset>0 && currentDepth<depthList[vOffset]){
                            vOffset--;
                        }
//                        if(vOffset>0 && currentOffset<depthList.size()-1 && depthList[currentOffset+1]<depthList[currentOffset])vOffset--;
//                        std::cout<<depthList[currentOffset]<<" "<<currentOffset<<" "<<vOffset<<" "<<depthList[vOffset]<<std::endl;
                        previousNode=std::next(dag.begin(), vOffset);
                        (*previousNode).second.push_back((*itDag).first);
                        (*itDag).second.push_back((*previousNode).first);
                    }
                }
                else if((*itDag).first.forward_slashA==0 && (*itDag).first.forward_slashB==1){
                    if(previousNode!=dag.end()){
                        (*previousNode).second.push_back((*itDag).first);
                        (*itDag).second.push_back((*previousNode).first);
                    }
                }
            }
            using value = std::ranges::range_value_t<decltype(edges)>;
            graph::vertex_id_t<G> N = static_cast<graph::vertex_id_t<G>>(size(graph::vertices(dagGraph)));
            using edge_desc  = graph::edge_info<graph::vertex_id_t<G>, true, void, int>;
            dagGraph.reserve_vertices(vertices.size());
            dagGraph.reserve_edges(edges.size());
            dagGraph.load_edges(edges, [](const value& val) -> edge_desc {
                    //std::cout<<"edge "<<std::get<0>(val)<<" "<<std::get<1>(val)<<" "<<std::get<2>(val)<<std::endl;
                return edge_desc{std::get<0>(val), std::get<1>(val), std::get<2>(val)};
              }, N);
            dagGraph.load_vertices(vertices, [&](tag_indexer& nm) {
                auto uid = static_cast<graph::vertex_id_t<G>>(&nm - vertices.data());
                //std::cout<<"uid "<<uid<<std::endl;
                return graph::copyable_vertex_t<graph::vertex_id_t<G>, tag_indexer>{uid, nm};
              });
    }
    
    bool Binder::findProlog(mio::mmap_source::const_iterator& it){
        if(std::strncmp(&(*it), "<?xml", 4)!=0) return false;
        std::advance(it, 5);
        skipOverWhiteSpaces(it);
        if(std::strncmp(&(*it), "version", 7)!=0) return false;
        std::advance(it, 7);
        skipOverWhiteSpaces(it);
        if((*it)!='=') return false;
        ++it;
        skipOverWhiteSpaces(it);
        mio::mmap_source::const_iterator itt =it;
        if((*itt)=='\'' && skipTo(itt, '\'')){
            ++it;
            version.assign(it, itt);
            it=itt;
            ++it;
        }
        else if((*itt)=='"' && skipTo(itt, '"')){
            ++it;
            version.assign(it, itt);
            it=itt;
            ++it;
        }
        else return false;
        skipOverWhiteSpaces(it);
        if(std::strncmp(&(*it), "encoding", 8)==0){
            std::advance(it, 8);
            skipOverWhiteSpaces(it);
            if((*it)=='='){
                ++it;
                skipOverWhiteSpaces(it);
                itt =it;
                if((*itt)=='\'' && skipTo(itt, '\'')){
                    ++it;
                    encoding.assign(it, itt);
                    it=itt;
                    ++it;
                }
                else if((*itt)=='"' && skipTo(itt, '"')){
                    ++it;
                    encoding.assign(it, itt);
                    it=itt;
                    ++it;
                }
            }
            
        }
        skipOverWhiteSpaces(it);
        if(std::strncmp(&(*it), "standalone", 10)==0){
            std::advance(it, 10);
            skipOverWhiteSpaces(it);
            if((*it)=='='){
                ++it;
                skipOverWhiteSpaces(it);
                itt =it;
                if((*itt)=='\'' && skipTo(itt, '\'')){
                    ++it;
                    if(std::strncmp(&(*it), "yes", 3)==0 || std::strncmp(&(*it), "no", 2)==0){
                        standalone=true;
                        it=itt;
                        ++it;
                    }
                }
                else if((*itt)=='"' && skipTo(itt, '"')){
                    ++it;
                    if(std::strncmp(&(*it), "yes", 3)==0 || std::strncmp(&(*it), "no", 2)==0){
                        standalone=true;
                        it=itt;
                        ++it;
                    }
                }
            }
            
        }
        
        skipOverWhiteSpaces(it);
        if(std::strncmp(&(*it), "?>", 2)!=0) return false;
        std::advance(it, 2);
        
        return true;
    }
    
    bool Binder::findSchemaComponents(std::u16string& utf16, std::u16string::const_iterator& it, std::vector<int>& depthList, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag, std::unordered_map<std::u16string, std::u16string_view>& schemaComponentMap){
        if((*it)=='<'){
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cv;
            size_t angleStart=std::distance(utf16.cbegin(), it);
            size_t angleSpace=0;
            size_t angleEnd=0;
            ++it;
            std::u16string::const_iterator ittmp =it;
            if(hasWhiteSpaceBeforeEndAngle(ittmp)){
            if(skipToWhiteSpace(ittmp)){
                angleSpace=std::distance(utf16.cbegin(), ittmp);
                std::u16string_view sv(&(*it), std::distance(it, ittmp));
                if(std::u16string::size_type pos;(pos=sv.find(u':'))!=std::u16string::npos){
                    schemaPrefix=std::u16string_view(&(*it), pos);
                    std::advance(it, pos);
                    sv=std::u16string_view(&(*it), std::distance(it, ittmp));
//                    if(sv.compare(u"schema")!=0)throw std::
                }
                it=ittmp;
                skipOverWhiteSpaces(it);
                while((*it)!=u'>' && (*it)!=std::char_traits<char>::eof()){
                    if(std::u16ncmp(&(*it), u"attributeFormDefault", 20)==0){
                        std::advance(it, 20);
                        skipOverWhiteSpaces(it);
                    }
                    else if(std::u16ncmp(&(*it), u"targetNamespace", 15)==0){
                        std::advance(it, 15);
                        skipOverWhiteSpaces(it);
                        if((*it)=='='){
                            if(skipTo(it, u'"')){
                                ++it;
                                ittmp =it;
                                if(skipTo(it, u'"')){
                                    ++it;
                                    //schemaComponentMap[u"targetNamespace"]=
//                                    std::cout<<"dist its=ite "<<std::distance(ittmp, it)<<std::endl;
                                    schemaComponentMap[u"targetNamespace"]=std::u16string_view(ittmp, it);
                                    //std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cv;
                                    //std::cout<<"targetNamespace? "<<cv.to_bytes(huh)<<std::endl;
                                    ++it;
                                }
                            }
                            skipOverWhiteSpaces(it);
                        }
                    }
                    else ++it;
                }
                if((*it)==u'>'){
                    angleEnd=std::distance(utf16.cbegin(), it);
                }
                
            }
            }
            else {
                it=ittmp;
                angleEnd=std::distance(utf16.cbegin(), it);
            }
            size_t forwardSlash=0;
            size_t forwardSlash2=0;
            vertices.push_back(tag_indexer{.index=vertices.size(), .angle_start=angleStart, .forward_slashA=forwardSlash, .space=angleSpace, .forward_slashB=forwardSlash2, .angle_end=angleEnd, .depth=nDepth});
            if(nDepth>=depthProfile.size())depthProfile.push_back(std::vector<size_t>{});
            depthProfile[nDepth].push_back(vertices.back().index);
            dag.push_back(std::make_pair(tag_indexer{.index=dag.size(), .angle_start=angleStart, .forward_slashA=forwardSlash, .space=angleSpace, .forward_slashB=forwardSlash2, .angle_end=angleEnd}, std::vector<tag_indexer>{}));
            depthList.push_back(0);
            depth++;
            nDepth++;
       }
         ++it;
        return true;
    }

}
