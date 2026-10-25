#pragma once
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <span>
#include <tuple>
#include <concepts>
#include <meta>
#include <variant>
#include <format>
#include <iostream>
#include <functional>
#include <ranges>

namespace sylvanmats::metaphrase::xsd{

    class GBackusNaurFormation{
        private:
        public:
        GBackusNaurFormation() = default;
        GBackusNaurFormation(const GBackusNaurFormation& orig) = delete;
        GBackusNaurFormation(GBackusNaurFormation&& orig) = delete;
        GBackusNaurFormation& operator=(const GBackusNaurFormation& orig) = delete;
        GBackusNaurFormation& operator=(GBackusNaurFormation&& orig) = delete;
        ~GBackusNaurFormation() = default;
        constexpr std::string_view operator ()(std::u8string_view g4Buffer){
            
            std::stringstream ss;
            // ss<<jsonBuffer;
            return ss.str();
        }
    };

}