# vim: set tabstop=4 autoindent:

#OBJS specifies which files to compile as part of the TEST
TEST_OBJS = test.cc rcpsp.cc

#OBJ_NAME specifes the name of our TEST executable
TEST_OBJ_NAME = test.out


#OBJS specifies which files to compile as part of the project
OBJS = main.cc rcpsp.cc

#OBJ_NAME specifes the name of our executable
OBJ_NAME = main.out

#CC specifies which compiler we're using
CC=g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w -std=c++11

#LINKER_FLAGS specifes the libraries we're linking against
LINKER_FLAGS = 

#This is the target compiles our executable
all : $(OBJS)
	$(CC)  $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -m64 -o $(OBJ_NAME)


test : $(TEST_OBJS)
	$(CC)  $(TEST_OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -m64 -o $(TEST_OBJ_NAME)
