cxxflags = -g -Wall -Wextra -std=c++11 -I /usr/include/freetype2 -I/home/afiefh/src/sfml_compiled/include
linkflags = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lfreetype -lharfbuzz -ljsoncpp -lGL -lGLEW -L/home/afiefh/src/sfml_compiled/lib

nightletters: main.o Font.o ComplexText.o wind.o lightning.o
	g++ ${cxxflags} main.o Font.o ComplexText.o GLCheck.o wind.o ${linkflags} -o nightletters

main.o: main.cpp bezier.hpp Font.hpp ComplexText.hpp Breeze.hpp wind.hpp lightning.hpp
	g++ -c ${cxxflags} main.cpp -o main.o

Font.o: Font.cpp Font.hpp
	g++ -c ${cxxflags} Font.cpp -o Font.o

GLCheck.o: GLCheck.cpp GLCheck.hpp
	g++ -c ${cxxflags} GLCheck.cpp -o GLCheck.o

ComplexText.o: ComplexText.cpp ComplexText.hpp
	g++ -c ${cxxflags}  ComplexText.cpp -o ComplexText.o

wind.o: wind.cpp wind.hpp bezier.hpp
	g++ -c ${cxxflags}  wind.cpp -o wind.o
	
lightning.o: lightning.cpp lightning.hpp
	g++ -c ${cxxflags}  lightning.cpp -o lightning.o
	
run: nightletters
	LD_LIBRARY_PATH=/home/afiefh/src/sfml_compiled/lib ./nightletters