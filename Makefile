ext=so
libprefix=lib
ifeq ($(OS),Windows_NT)
  ext=dll
  libprefix=
endif
LD=ld.exe

MODULE_DIRECTORY ?= ./cpp_modules/

-include build/src/io/xml/Binder.d
-include build/src/io/xml/Path.d

all: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26  -Iinclude -MMD -Wl,--allow-multiple-definition
all: LDFLAGS= -shared  -L$(MODULE_DIRECTORY)/antlr4/runtime/Cpp/run/lib -lantlr4-runtime
ifeq ($(OS),Windows_NT)
all: LDFLAGS=" -Wl,--export-all-symbols ${LDFLAGS}"
endif
all: build/src/io/xml/Path.o build/src/io/xml/Binder.o build/src/io/xslt/Morpher.o build/src/parsing/XMLLexer.o build/src/parsing/XMLParser.o 
	@mkdir -p $(@D)
	#ld --help
	$(CXX) $(LDFLAGS) -o $(libprefix)xmlthresher.$(ext) $(wildcard build/src/*.o) $(wildcard build/src/parsing/*.o) $(wildcard build/src/io/xpath/*.o) $(wildcard build/src/io/xml/*.o) $(wildcard build/src/io/xslt/*.o) 

build/src/io/xml/Path.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -Isrc/parsing -I$(MODULE_DIRECTORY)graph-v2/include -I$(MODULE_DIRECTORY)zlib/dist/include -I$(MODULE_DIRECTORY)antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/io/xml/Path.o: src/io/xml/Path.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/io/xml/Path.o src/io/xml/Path.cpp
	
build/src/io/xml/Binder.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I$(MODULE_DIRECTORY)mio/include -I$(MODULE_DIRECTORY)graph-v2/include -I$(MODULE_DIRECTORY)zlib/dist/include -MMD -MP
build/src/io/xml/Binder.o: src/io/xml/Binder.cpp
	@mkdir -p $(@D)
	g++ --version
	$(CXX) $(CXXFLAGS) -c -o build/src/io/xml/Binder.o src/io/xml/Binder.cpp

build/src/io/xslt/Morpher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I$(MODULE_DIRECTORY)mio/include -I$(MODULE_DIRECTORY)graph-v2/include -I$(MODULE_DIRECTORY)zlib/dist/include -MMD -MP
build/src/io/xslt/Morpher.o: src/io/xslt/Morpher.cpp
	@mkdir -p $(@D)
	g++ --version
	$(CXX) $(CXXFLAGS) -c -o build/src/io/xslt/Morpher.o src/io/xslt/Morpher.cpp

build/src/parsing/XMLLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I./src/parsing -I$(MODULE_DIRECTORY)zlib/dist/include -I$(MODULE_DIRECTORY)antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/XMLLexer.o: ./src/parsing/XMLLexer.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/XMLLexer.o ./src/parsing/XMLLexer.cpp

build/src/parsing/XMLParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I./src/parsing -I$(MODULE_DIRECTORY)zlib/dist/include -I$(MODULE_DIRECTORY)antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
build/src/parsing/XMLParser.o: ./src/parsing/XMLParser.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/XMLParser.o ./src/parsing/XMLParser.cpp

