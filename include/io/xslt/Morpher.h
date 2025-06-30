
#pragma once

#include <cstdio>
#include <string>
#include <string_view>
#include <functional>

#include "io/xml/Binder.h"

namespace sylvanmats::io::xslt{
    class Morpher{
    protected:
        xml::Binder xmlBinder;
        xml::Binder xsltBinder;
    public:
        Morpher() = delete;
        Morpher(std::string xmlPath, std::string xsltPath);
        Morpher(const Morpher& orig) = delete;
        virtual ~Morpher() = default;

        void operator()(std::function<void(std::u16string& utf16)> apply);
    };
}