
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#include <concepts>

#define protected public

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

TEST_SUITE("paths"){
TEST_CASE("test path as container")
{
    static_assert(std::copy_constructible<sylvanmats::io::xml::Path<std::u16string>>);
//    static_assert(std::input_or_output_iterator<sylvanmats::io::xml::Path<std::u16string>>::iterator);
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
    sylvanmats::io::xml::Path<std::u16string> path(u"/xsd:schema/xsd:element[@name]");
    CHECK_EQ(path.size(), 2);
    sylvanmats::io::xml::Path<std::u16string>&& path2=u"/xsd:schema/xsd:element[@name]"_xp;
    CHECK_EQ(path2.size(), 2);
    for(sylvanmats::io::xml::Path<std::u16string>::iterator it=path2.begin();it!=path2.end();it++){
        std::cout<<" "<<cv.to_bytes(*it)<<std::endl;
    }
}
}