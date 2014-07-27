PROJECT     = tipee
SHELL       = /bin/sh
TARGET      = tipee
OBJS        = main.o ExpressionManager.o Expression.o String.o \
			  BluetoothSocket.o BluetoothServerSocket.o \
			  Message.o Movement.o MessageManager.o

LIBS        = 

CC          = gcc
LD          = gcc
RM          = /bin/rm -rf

CPPFLAGS    = `pkg-config --cflags gtk+-3.0` -fpermissive
LDFLAGS     = `pkg-config --libs gtk+-3.0` -lbluetooth -lstdc++

.SUFFIXES: .cpp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CC) $(CPPFLAGS) -c $< -o $*.o 

clean:
	rm *.o $(TARGET)

srcpack:
	tar cvf packed/$(PROJECT).src.tar \
			*.cpp *.h \
            Makefile \
			*.json *.html
	gzip packed/$(PROJECT).src.tar
