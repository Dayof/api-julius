LIBSENT=libsent
LIBJULIUS=libjulius

SOURCE_DIR=src/
INCLUDE_DIR=include/
OBJ_DIR=obj

CC=g++
CFLAGS=-g -O2

_DEPS=RecoResult.h SREngine.h
DEPS=$(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ=RecoResult.o SREngine.o main.o
OBJ=$(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

_SOURCE=RecoResult.cpp SREngine.cpp main.cpp
SOURCE=$(patsubst %,$(SOURCE_DIR)/%,$(_SOURCE))

####
#### When using system-installed libraries
####
# CPPFLAGS=`libjulius-config --cflags` `libsent-config --cflags`
# LDFLAGS=`libjulius-config --libs` `libsent-config --libs`

####
#### When using within-package libraries
####
CPPFLAGS=-I$(LIBJULIUS)/include -Iinclude -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags` `$(LIBJULIUS)/libjulius-config --cflags`
LDFLAGS= -L$(LIBJULIUS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs`

############################################################

all: main.o RecoResult.o SREngine.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o main main.o RecoResult.o SREngine.o $(LDFLAGS)

main.o: main.cpp $(DEPS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) main.cpp $(LDFLAGS)

RecoResult.o: $(SOURCE_DIR)RecoResult.cpp $(INCLUDE_DIR)RecoResult.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SOURCE_DIR)RecoResult.cpp $(LDFLAGS)

SREngine.o: $(SOURCE_DIR)SREngine.cpp $(DEPS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(SOURCE_DIR)SREngine.cpp $(LDFLAGS)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) main


# main: main.cpp
# 	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -o main main.cpp $(LDFLAGS)
