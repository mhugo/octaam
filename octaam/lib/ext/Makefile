AAM_OBJ=update_h_copt.oct update_h_rob_copt.oct warp_2d_copt.oct compute_jacobian_aux_copt.oct

INC=-I$(shell octave-config -p OCTINCLUDEDIR) -fPIC -mieee-fp -ggdb
LIB= -shared -Wl,-Bsymbolic -L$(shell octave-config -p OCTLIBDIR) -loctave

INSTDIR=$(shell octave-config -p OCTFILEDIR)

CC=g++

all: $(AAM_OBJ)

clean:
	rm -f $(OBJ) *.o


%.o: %.cc
	$(CC) -c $< $(INC) -o $@

%.oct: %.o
	$(CC) $(LIB) $< -o $@

install:
	cp $(AAM_OBJ) $(INSTDIR)
