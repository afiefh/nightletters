cxxflags = -g -Wall -Wextra -std=c++11 -I /usr/include/freetype2 -I/home/afiefh/src/sfml_compiled/include
linkflags = -lsfml-graphics -lsfml-window -lsfml-system -lfreetype -lharfbuzz -lGL -lGLEW -L/home/afiefh/src/sfml_compiled/lib

nightletters: main.o Font.o ComplexText.o
	g++ ${cxxflags} main.o Font.o ComplexText.o GLCheck.o ${linkflags} -o nightletters

main.o: main.cpp bezier.hpp Font.hpp ComplexText.hpp
	g++ -c ${cxxflags} main.cpp -o main.o

Font.o: Font.cpp Font.hpp
	g++ -c ${cxxflags} Font.cpp -o Font.o

GLCheck.o: GLCheck.cpp GLCheck.hpp
	g++ -c ${cxxflags} GLCheck.cpp -o GLCheck.o

ComplexText.o: ComplexText.cpp ComplexText.hpp
	g++ -c ${cxxflags}  ComplexText.cpp -o ComplexText.o
	
run: nightletters
	LD_LIBRARY_PATH=/home/afiefh/src/sfml_compiled/lib ./nightletters