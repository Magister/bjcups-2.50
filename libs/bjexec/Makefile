INSTALL_DIR = /usr/lib
INSTALL_HDR = /usr/include
INSTALL_LIB = /usr/bin/install -c -m 644
INSTALL_DAT = /usr/bin/install -c -m 644

NAME	= bjexec
LIBNAME = lib$(NAME).a
HDRNAME = bjexec.h

CFLAGS  = -I../paramlist -c -O2 -Wall
LFLAGS  =

OBJS = bjexec.o
LIBS =

.c.o : 
	$(CC) $(CFLAGS) $(HDRPATH) $<

all : $(LIBNAME)

$(LIBNAME) : $(OBJS)
	ar crv $(LIBNAME) $(OBJS)

clean :
	rm -rf *.o $(LIBNAME)

install :
	$(INSTALL_LIB) $(LIBNAME) $(INSTALL_DIR)
	$(INSTALL_DAT) $(HDRNAME) $(INSTALL_HDR)

# Header file dependencies
bjexec.o : bjexec.c $(HDRNAME)

