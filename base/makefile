SOURCE := $(wildcard *.cpp)
OBJS   := $(patsubst %.cpp,%.o,$(SOURCE))
CFLAGS := -std=c++11 -g -Wall
LIBS   := -lpthread
Loggingtest: $(OBJS) test.o
	g++ $(CFLAGS) -o test $^ $(LIBS)
clean : 
	find . -name '*.o' | xargs rm -f
