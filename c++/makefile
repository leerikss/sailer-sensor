CPP		:= g++
CPPFLAGS	:= -Wall
LIBS		:= -lconfig++
OBJECTS		:= sailersensor.o lsmpoller.o lsm303dlhc/lsm303dlhc.h

sailersensor: $(OBJECTS)
	$(CPP) $(OBJECTS) -o sailersensor.bin

sailersensor.o: lsmpoller.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c sailersensor.cpp -o sailersensor.o

lsmpoller.o: lsmpoller.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c lsmpoller.cpp -o lsmpoller.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
