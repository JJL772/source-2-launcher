
FLAGS32=-m32
DEFINES=-D_UNIX -DUNIX
LIBS=-lc -lm -lrt -ldl
SRC=$(wildcard src/*.cc)

all:
	mkdir -p bin 
	g++ -o bin/source2-rev-eng $(FLAGS) $(DEFINES) $(SRC) $(LIBS)

clean:
	rm -rf bin

install: 
	cp bin/source2-rev-eng /usr/local/bin/