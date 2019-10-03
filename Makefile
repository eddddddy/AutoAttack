CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -Ofast -MMD
EXEC = auto
OBJECTS = main.o skill.o state.o node.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
