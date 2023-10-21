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

#include "io/xml/Binder.h"

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

Binder::Binder(std::string xmlPath, std::string xsdPath) {
    try {
        const int fdXsd = open(xmlPath.c_str(), O_RDONLY);
        mio::mmap_source mmapXsd(fdXsd, 0, mio::map_entire_file);
        mio::mmap_source::const_iterator itg=mmapXsd.begin();
        if(!findProlog(itg))itg=mmapXsd.begin();
        else{
            std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> utf16conv;
            utf16 = utf16conv.from_bytes(&(*itg));
            std::u16string::const_iterator it = utf16.cbegin();
            int count=0;
            skipOverWhiteSpaces(it);
            findSchemaComponents(it);
            skipOverWhiteSpaces(it);
            std::u16string::const_iterator temp=it;
            std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cv;
            size_t depth=0;
            while(temp!=utf16.end()){
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
                        dag.push_back(std::make_pair(tag_indexer{.index=dag.size(), .angle_start=angleStart, .forward_slashA=forwardSlash, .space=angleSpace, .forward_slashB=forwardSlash2, .angle_end=angleEnd}, std::vector<tag_indexer>{}));
                        if(forwardSlash==1){
                            depthList.push_back(depth);
                            if(depth>0)depth--;
                        }
                        else if(forwardSlash2==1){
                            depth++;
                            depthList.push_back(depth);
                            if(depth>0)depth--;
                        }
                        else {
                            depth++;
                            depthList.push_back(depth);
                        }
                        ++temp;
                    }
                    //std::cout<<"angle bracket? "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<" "<<cv.to_bytes(*temp++)<<std::endl;
                }
                else ++temp;
            }
//            std::string depthText=fmt::format("{}\n", depthList);
//            std::cout<<depthText;
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
        }
        
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
}

    void Binder::operator()(std::function<void(std::u16string& utf16, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag)> apply){
        apply(utf16, dag);
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
    
    bool Binder::findSchemaComponents(std::u16string::const_iterator& it){
        if((*it)=='<'){
            size_t angleStart=std::distance(utf16.cbegin(), it);
            size_t angleSpace=0;
            size_t angleEnd=0;
            ++it;
            std::u16string::const_iterator ittmp =it;
            if(skipToWhiteSpace(ittmp)){
                angleSpace=std::distance(it, ittmp);
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
            size_t forwardSlash=0;
            size_t forwardSlash2=0;
            dag.push_back(std::make_pair(tag_indexer{.index=dag.size(), .angle_start=angleStart, .forward_slashA=forwardSlash, .space=angleSpace, .forward_slashB=forwardSlash2, .angle_end=angleEnd}, std::vector<tag_indexer>{}));
            depthList.push_back(0);
        }
        ++it;
        return true;
    }

}
