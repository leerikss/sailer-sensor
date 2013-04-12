CPP		:= g++
CPPFLAGS	:= -Wall
LIBS		:= -lconfig++ -lboost_thread
OBJECTS		:=  sailersensor.o lsmpoller.o

sailersensor: $(OBJECTS)
	$(CPP) $(LIBS) $(OBJECTS) -o sailersensor.bin

sailersensor.o: sailersensor.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c sailersensor.cpp -o sailersensor.o

lsmpoller.o: lsmpoller.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c lsmpoller.cpp -o lsmpoller.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
