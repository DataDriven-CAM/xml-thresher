
-include build/src/io/xml/Binder.d
-include build/src/io/xml/Path.d

all: LDFLAGS= -fuse-ld=gold -L../cpp_modules/antlr4/runtime/Cpp/run/lib -L`pwd`/../cpp_modules/fmt/dist/lib -L../ -lfmt -lantlr4-runtime -lxmlthresher -ldl -pthread 
all: build/src/path_test.o build/src/binding_test.o test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o) 

#build/src/parsing/STLexer.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I../src/parsing -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
#build/src/parsing/STLexer.o: ../src/parsing/STLexer.cpp 
#	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/STLexer.o ../src/parsing/STLexer.cpp

#build/src/parsing/STParser.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -Iinclude -Isrc -I../src/parsing -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD
#build/src/parsing/STParser.o: ../src/parsing/STParser.cpp 
#	$(CXX) $(CXXFLAGS) -c -o build/src/parsing/STParser.o ../src/parsing/STParser.cpp

build/src/path_test.o: CXXFLAGS= -fconcepts-diagnostics-depth=2 -DNDEBUG -O3 -pthread -std=c++26 -I../include -I../src -I../include/io/xpath -I../src/parsing -I../cpp_modules/doctest -I../cpp_modules/mio/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD -Wl,--allow-multiple-definition
build/src/path_test.o: src/path_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/path_test.o src/path_test.cpp

build/src/binding_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26 -I../include -I../cpp_modules/doctest -I../src -I../src/parsing -I../cpp_modules/mio/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD -Wl,--allow-multiple-definition
build/src/binding_test.o: src/binding_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/binding_test.o src/binding_test.cpp

test: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26 -I../include -I../src -I../cpp_modules/doctest -I../cpp_modules/mio/include -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD -Wl,--allow-multiple-definition
test: src/test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp 

clean:
