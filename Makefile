CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -Ofast -MMD
EXEC = auto
OBJECTS = main.o skill.o state.o node.o explore.o memcheck.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -lncurses

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
