ifeq ($(OS),Windows_NT)
	cxxflags = -g -Wall -Wextra -std=c++11 -I /usr/include/freetype2 -I/home/afiefh/src/SFML-2.2/include -I/usr/include -I/usr/local/include -I/usr/local/include/freetype2
	linkflags = -L/home/afiefh/src/SFML-2.2/lib -L/home/afiefh/src/SFML-2.2/bin -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lfreetype -lharfbuzz -lopengl32 -lglew32 -ljsoncpp -L/usr/lib -L/usr/local/lib  -L/home/afiefh/src/SFML-2.2/lib
else
	cxxflags = -g -Wall -Wextra -std=c++11 -I /usr/include/freetype2 #-I/home/afiefh/src/sfml_compiled/include
	linkflags = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lfreetype -lharfbuzz -ljsoncpp -lGL -lGLEW #-L/home/afiefh/src/sfml_compiled/lib
endif


#Define the source files, object files and dependency files
SOURCES = main.cpp ComplexFont.cpp ComplexText.cpp wind.cpp lightning.cpp SoundManager.cpp GLCheck.cpp utilities.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
DEPS = $(OBJECTS:.o=.d)

# define the name of the executable
nightletters: ${OBJECTS}
	g++ ${cxxflags} ${OBJECTS} ${linkflags} -o nightletters

#include the dependency files
-include $(DEPS)

#compile the object files
%.o: %.cpp
	g++ ${cxxflags} -MP -MMD -c $< -o $@

run: nightletters
ifeq ($(OS),Windows_NT)
		rm -rf build/graphic
		rm -rf build/audio
		cp nightletters.exe build/bin
		cp *.json build/bin
		cp -r ../graphic build/
		cp -r ../audio build/
		cd build/bin; ./nightletters.exe
else
		LD_LIBRARY_PATH=/home/afiefh/src/sfml_compiled/lib ./nightletters
endif

dist: nightletters
ifeq ($(OS),Windows_NT)
		rm -f Nightletters.zip
		zip -r -9 Nightletters.zip build
else
		echo "Can't make dist on Linux"
endif
