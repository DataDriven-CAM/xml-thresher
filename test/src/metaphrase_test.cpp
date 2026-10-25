#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <locale>
#include <codecvt>
#include <format>

#define protected public
#include "publishing/formatting.h"

#include "metaphrase/xsd/GBackusNaurFormation.h"

TEST_SUITE ("formatting"){
    
TEST_CASE("test xsd metaphrase as gbnf"){
    sylvanmats::metaphrase::xsd::GBackusNaurFormation gBackusNaurFormation;

}

}