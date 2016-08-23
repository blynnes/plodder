CC=/home/vagrant/CHIP-buildroot/output/host/usr/bin/arm-linux-gnueabihf-g++
CFLAGS=-I. `/home/vagrant/CHIP-buildroot/output/host/usr/bin/arm-linux-gnueabihf-pkg-config --cflags --libs glib-2.0` `/home/vagrant/CHIP-buildroot/output/host/usr/bin/arm-linux-gnueabihf-pkg-config --cflags --libs alsa`

DEPS = GPIOManip.h mic.h
OBJ = plodder.o mic.o GPIOManip.o

default: plodder

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

plodder: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core plodder 
