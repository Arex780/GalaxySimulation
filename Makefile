# Environment variables
MKDIR=mkdir
CCADMIN=CCadmin
CC=gcc
CCC=g++
CXX=g++

# Binary directory
BINARYDIR=bin

# Object directory
OBJECTDIR=obj

# Object files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/DisplayWindow.o \
	${OBJECTDIR}/Euler.o \
	${OBJECTDIR}/Heun.o \
	${OBJECTDIR}/IDisplay.o \
	${OBJECTDIR}/IIntegrator.o \
	${OBJECTDIR}/IModel.o \
	${OBJECTDIR}/NBody.o \
	${OBJECTDIR}/Octree.o \
	${OBJECTDIR}/Particles.o \
	${OBJECTDIR}/Quadtree.o \
	${OBJECTDIR}/RK4.o \
	${OBJECTDIR}/Vectors.o

# Compilers flags
CFLAGS=
CCFLAGS=-std=c++11
CXXFLAGS=-std=c++11

# Link libraries
LDLIBSOPTIONS=-lSDL -lGL -lGLU -lX11 -ljsoncpp

# Build targets
${BINARYDIR}/main: ${OBJECTFILES}
	${MKDIR} -p ${BINARYDIR}
	${LINK.cc} -o ${BINARYDIR}/main ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/DisplayWindow.o: DisplayWindow.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DisplayWindow.o DisplayWindow.cpp

${OBJECTDIR}/Euler.o: Integrators/Euler.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Euler.o Integrators/Euler.cpp

${OBJECTDIR}/Heun.o: Integrators/Heun.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Heun.o Integrators/Heun.cpp

${OBJECTDIR}/IDisplay.o: Interfaces/IDisplay.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IDisplay.o Interfaces/IDisplay.cpp

${OBJECTDIR}/IIntegrator.o: Interfaces/IIntegrator.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IIntegrator.o Interfaces/IIntegrator.cpp

${OBJECTDIR}/IModel.o: Interfaces/IModel.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/IModel.o Interfaces/IModel.cpp

${OBJECTDIR}/NBody.o: Models/NBody.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NBody.o Models/NBody.cpp

${OBJECTDIR}/Octree.o: Trees/Octree.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Octree.o Trees/Octree.cpp

${OBJECTDIR}/Particles.o: Structs/Particles.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Particles.o Structs/Particles.cpp

${OBJECTDIR}/Quadtree.o: Trees/Quadtree.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Quadtree.o Trees/Quadtree.cpp

${OBJECTDIR}/RK4.o: Integrators/RK4.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RK4.o Integrators/RK4.cpp

${OBJECTDIR}/Vectors.o: Structs/Vectors.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Vectors.o Structs/Vectors.cpp