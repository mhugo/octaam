INC=-I$(shell octave-config -p OCTINCLUDEDIR) -fPIC -mieee-fp -ggdb
LIB= -shared -Wl,-Bsymbolic -L$(shell octave-config -p OCTLIBDIR) -loctave -lGL -lglut

INSTDIR=$(shell octave-config -p LOCALOCTFILEDIR)

CC=g++

all: fd_viola_jones.oct

clean:
	rm -f $(OBJ) *.o


fd_viola_jones.oct: fd_viola_jones.o face_detector.o
	$(CC) $(LIB) $(shell pkg-config opencv --libs) fd_viola_jones.o face_detector.o -o $@

face_detector.o: face_detector.cc face_detector.h
	$(CC) $(INC) $(shell pkg-config opencv --cflags) -c $< -o $@
fd_viola_jones.o: fd_viola_jones.cc
	$(CC) $(INC) $(shell pkg-config opencv --cflags) -c $< -o $@

install:
	cp $(OBJ) $(INSTDIR)
