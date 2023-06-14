export CFLAGS = -DVOLUME_SIZE=40000 -DNAME_SIZE=256

all: filesystem

filesystem: main.o filesystem.o
	$(CC) -o filesystem main.o filesystem.o $(LDFLAGS)

main.o: src/main.c
	$(CC) -c -o main.o src/main.c -I./include $(CFLAGS)

filesystem.o: src/filesystem.c
	$(CC) -c -o filesystem.o src/filesystem.c -I./include $(CFLAGS)

clean:
	rm -f filesystem *.o