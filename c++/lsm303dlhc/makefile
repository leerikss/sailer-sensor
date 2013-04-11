CPP		:= g++
CPPFLAGS	:= -Wall
LIBS		:= -lconfig++
OBJECTS		:= lsmout.o lsm303dlhc.o I2C_bus.o smbus.o

lsmout: $(OBJECTS)
	$(CPP) $(LIBS) $(CPPFLAGS) $(OBJECTS) -o lsmout.bin

lsmout.o: lsmout.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c lsmout.cpp -o lsmout.o

lsm303dlhc.o: lsm303dlhc.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c lsm303dlhc.cpp -o lsm303dlhc.o

I2C_bus.o: I2C_bus.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c I2C_bus.cpp -o I2C_bus.o

smbus.o: smbus.c
	$(CPP) $(LIBS) $(CPPFLAGS) -c smbus.c -o smbus.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
