# http://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = gcc
CFLAGS  = -g -Wall
PROJECT = mytester
LIB = OBJECTS

#PATHS = .\project\main\ .\project\utils\

#INCLUDE = $(patsubst(%,%-I,$(PATHS)))	
	

	
INCPATH  = project/main
INCPATH += project/utils

INCDIR  = $(patsubst %,-I%,$(INCPATH))

SRC = 	project/main/main.c	
SRC +=	project/utils/util.c 

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#		
OBJ = $(SRC:.c=.o)

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
all: createdirs $(PROJECT)
	mv $(OBJ) $(LIB)
	@echo Project succesfully compiled

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
$(PROJECT):  $(OBJ)
	$(CC) $(CFLAGS) $(INCDIR) -o $(PROJECT) $(OBJ)

createdirs : 
	mkdir -p $(LIB)/

%o : %c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@	
	
# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
#util.o:  project/utils/util.c 
#	$(CC) $(CFLAGS) -Wa,-ahlms=$(<:.c=.lst) $(INCLUDE) -c project\utils\util.h project\utils\util.c
#	@echo util.o created....
#
# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#
#main.o:  project/main/main.c 
#	$(CC) $(CFLAGS) $(INCLUDE) -c project/main/main.c
#	@echo main.o created....
#	mv main.o $(LIB)
# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#

clean: 
	$(RM) $(OBJ) $(PROJECT).exe 
	@rm -fr $(LIB)
	
	


	
	
#tester : main.c
#	gcc -o main.c -$@

#%.o : .c
#	gcc -o $@	
#clean: 
#	# Cleaning 
#	@rm -f main.o


# in remove : f = force, r recursive