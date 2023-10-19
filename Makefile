ext=so
libprefix=lib
ifeq ($(OS),Windows_NT)
  ext=dll
  libprefix=
endif
LD=ld.exe

all: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++23  -Iinclude -MMD -Wl,--allow-multiple-definition
all: LDFLAGS= -shared  -Wl,--allow-multiple-definition -L`pwd`/cpp_modules/fmt/dist/lib -lfmt
ifeq ($(OS),Windows_NT)
all: LDFLAGS=" -Wl,--export-all-symbols ${LDFLAGS}"
endif
all: build/src/io/xml/Binder.o build/src/parsing/XMLLexer.o build/src/parsing/XMLParser.o
	@mkdir -p $(@D)
	#ld --help
	$(CXX) $(LDFLAGS) -o $(libprefix)xmlthresher.$(ext) $(wildcard build/src/*.o) $(wildcard build/src/parsing/*.o)  $(wildcard build/src/io/xml/*.o) 

build/src/io/xml/Binder.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++23 -Iinclude -Isrc -I./cpp_modules/mio/include -I./cpp_modules/fmt/dist/include -MMD
build/src/io/xml/Binder.o: src/io/xml/Binder.cpp 
	@mkdir -p $(@D)
	g++ --version
	$(CXX) $(CXXFLAGS) -c -o build/src/io/xml/Binder.o src/io/xml/Binder.cpp

build/src/parsing/XMLLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I./src/parsing -I./cpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/XMLLexer.o: ./src/parsing/XMLLexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/XMLLexer.o ./src/parsing/XMLLexer.cpp

build/src/parsing/XMLParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I./src/parsing -I./cpp_modules/zlib/dist/include -I./cpp_modules/antlr4/runtime/Cpp/run/usr/local/include/antlr4-runtime -MMD
build/src/parsing/XMLParser.o: ./src/parsing/XMLParser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/XMLParser.o ./src/parsing/XMLParser.cpp

