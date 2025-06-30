

#include "io/xslt/Morpher.h"

namespace sylvanmats::io::xslt{
    Morpher::Morpher(std::string xmlPath, std::string xsltPath) : xmlBinder (xml::Binder(xmlPath, "")), xsltBinder (xml::Binder(xsltPath, "")) {
    }

    void Morpher::operator()(std::function<void(std::u16string& utf16)> apply){
        xmlBinder([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
        xsltBinder([](std::u16string& utf16, sylvanmats::io::xml::G& dagGraph){});
        sylvanmats::io::xml::Path<std::u16string> path(u"/xsl:stylesheet/xsl:output[@method]");
        char method=0;
        xsltBinder(path, [&method](std::u16string_view& value){method=(value.compare(u"xml")==0)? 1: 0;});
        std::cout<<"method "<<static_cast<int>(method)<<std::endl;
    }
}
