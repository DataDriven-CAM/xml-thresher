/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Binder.h
 * Author: roger
 *
 * Created on February 22, 2020, 7:58 AM
 */

#pragma once

#include <stdlib.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <exception>
#include <string>
#include <string_view>
#include <locale>
#include <codecvt>
#include <cstring>
#include <cwchar>
#include <iomanip>
#include <sstream>
#include <memory>
#include <fstream>
#include <functional>
#include <chrono>
#include <format>
#include <utility>


#include <fcntl.h>
#include "mio/mmap.hpp"
#include <algorithm>
#include <unordered_map>
#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/ranges.h"


/*template <>
struct std::formatter<std::vector<size_t>> : std::formatter<size_t>{
    constexpr auto parse(format_parse_context& ctx)
    {
        value_format= "{:";        
        for (auto it= begin(ctx); it != end(ctx); ++it) {
            char c= *it;
            if (c == 'c')
                curly= true;
            else
                value_format+= c;
            if (c == '}')
                return it;
        }
        return end(ctx);
    }
    
    template <typename FormatContext>
    auto format(const std::vector<size_t>& v, FormatContext& ctx) const{
        const auto&& buf=ctx.out();
        if(curly){
            constexpr char* fmt={"{{"};
            std::format_to(ctx.out(), fmt);
        }
        else{
            constexpr char* fmt{"["};
            std::format_to(ctx.out(), fmt);            
        }
        if (v.size() > 0){
            const std::string_view vf=value_format;
            size_t d=v[0];
            std::format_to(ctx.out(), vf, d);
        }
        for (int i= 1; i < v.size(); ++i)
            std::vformat_to(buf, ", " + value_format, v[i]);
        if(curly){
            constexpr char* fmt={"}}"};
            return std::format_to(ctx.out(), fmt);
        }
        else{
            constexpr char* fmt={"]"};
            return std::format_to(ctx.out(), fmt);
        }
    }
    // ...
    bool        curly{false};
    std::string value_format;
};*/

template<class Facet>
struct deletable_facet : Facet
{
    template<class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

namespace sylvanmats::io::xml{
    
    struct tag_indexer{
        size_t index=0;
        size_t angle_start=0;
        size_t forward_slashA=0;
        size_t space=0;
        size_t forward_slashB=0;
        size_t angle_end=0;
    };

template<typename T>
constexpr size_t range2(std::array<T, 2> arg) {
    return arg[1]-arg[0]-1;
}

template<typename T, typename ... Args>
constexpr size_t range2(std::array<T, 2> arg, Args... args) {
    if(sizeof ...(Args)==0){
	return range2(arg);
    }
    else{
        T t=arg[1]-arg[0]-1;
        t+=range2(args...);
        return t;
    }
}

template<typename T, size_t N> 
constexpr const std::array<T, N> the_array(){
    return std::array<T, N>{};
}
template<typename T, typename ... Args>
constexpr auto make_charset_array(Args ... args)
{
    const std::array result=the_array<T, range2(args...)>();
    for (const auto& arg: {args...}) {
        size_t i=0;
        for(T c=arg[0];c<=arg[1];c++){
            result[i++]=c;
        }
        //result[i]= arg;
    }
//    for (T i = N; i <= M; ++i)
//        result[(i-N)]=i;
    return result;
}

template<typename T>
constexpr auto range(T N, T M)
{
    std::array<T, 2> result;
    result[0]=N;
    result[1]=M;
    return result;
}

class Binder {
protected:
    std::string version="";
    std::string encoding="UTF-8";
    bool standalone=false;
    std::u16string utf16;
    std::u16string_view schemaPrefix;
    std::unordered_map<std::u16string, std::u16string_view> schemaComponentMap;
    std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>> dag;
    std::vector<int> depthList;
    std::u16string xsdUTF16;
    std::u16string_view xsdSchemaPrefix;
    std::unordered_map<std::u16string, std::u16string_view> xsdSchemaComponentMap;
    std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>> xsdDAG;
    std::vector<int> xsdDepthList;
//    constexpr static std::array arr = make_charset_array<char16_t>(range(u'a', u'z'), range(u'_', u'_'), range(u'A', u'Z'));
//    constexpr static std::array arr = make_charset_array(std::integral_constant<char16_t, u'a'>{}, std::integral_constant<char16_t, u'z'>{});
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> utf16conv;
    
public:
    Binder() = delete;
    Binder(std::string xmlPath, std::string xsdPath);
    Binder(const Binder& orig) = delete;
    virtual ~Binder() = default;
    
    void operator()(std::function<void(std::u16string& utf16, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag)> apply);
    
    std::u16string findAttribute(std::u16string name, size_t start, size_t end);

private:
    void initializeGraph(std::u16string& utf16, std::vector<int>& depthList, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag, std::unordered_map<std::u16string, std::u16string_view>& schemaComponentMap);
    bool findProlog(mio::mmap_source::const_iterator& it);
    bool findSchemaComponents(std::u16string::const_iterator& it, std::vector<int>& depthList, std::vector<std::pair<tag_indexer, std::vector<tag_indexer>>>& dag, std::unordered_map<std::u16string, std::u16string_view>& schemaComponentMap);
    inline void skipOverWhiteSpaces(mio::mmap_source::const_iterator& it){
        while((*it)!=std::char_traits<char>::eof() && std::isspace((*it))){
            ++it;
        }
    };
    
    inline bool skipTo(mio::mmap_source::const_iterator& it, char c){
        size_t n=1;
        while((*(it+n))!=std::char_traits<char>::eof() && (*(it+n))!=c){
            n++;
        }
        if((*(it+n))==std::char_traits<char>::eof()) return false;
        std::advance(it, n);
        return true;
    };
    
    ////wide char
    inline bool inCharSet(char16_t c){ return (c>=0x1 && c<=0xD7FF) | (c>=0xE000 && c<=0xFFFD) | (c>=0x10000 && c<=0x10FFFF);};
    inline void skipOverWhiteSpaces(std::u16string::const_iterator& it){
        while((*it)!=std::char_traits<char16_t>::eof() && std::isspace((*it))){
            ++it;
        }
    };
    inline bool skipToWhiteSpace(std::u16string::const_iterator& it){
        while((*it)!=std::char_traits<char16_t>::eof() && !std::isspace((*it))){
            ++it;
        }
        return (*it)!=std::char_traits<char16_t>::eof();
    };
    inline bool skipTo(std::u16string::const_iterator& it, char16_t c){
        size_t n=1;
        while((*(it+n))!=std::char_traits<char16_t>::eof() && (*(it+n))!=c){
            n++;
        }
        if((*(it+n))==std::char_traits<char16_t>::eof()) return false;
        std::advance(it, n);
        return true;
    };
    
};

}
