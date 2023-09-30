CC=g++
INCLUDE=-Iinclude
CFLAGS=-std=c++17 -g
LDFLAGS=-lglfw -lvulkan -ldl -lX11 -lXxf86vm -lXrandr -lXi
OBJS=obj/age_window.o obj/age_engine.o obj/age_device.o obj/age_swapchain.o

all: bin/age

clean:
	rm -f bin/* lib/* obj/*
	
redo: clean bin/age
	./bin/age

test: clean bin/age
	valgrind --leak-check=full --track-origins=yes bin/age

run: bin/age
	./bin/age

#bin/main: src/main.cc
#	$(CC) $(CFLAGS) $< -o $@ $(LIB)

bin/age: obj/main.o $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ $(LDFLAGS)

obj/%.o: src/%.cc
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@ $(LDFLAGS)

