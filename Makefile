export CFLAGS += -DVOLUME_SIZE=40000 -DNAME_SIZE=256
export DIR1 += bin

all: filesystem clean_obj_files

filesystem: check_dir main.o filesystem.o
	$(CC) -o bin/filesystem main.o filesystem.o $(LDFLAGS)

main.o: src/main.c
	$(CC) -c -o main.o src/main.c -I./include $(CFLAGS)

filesystem.o: src/filesystem.c
	$(CC) -c -o filesystem.o src/filesystem.c -I./include $(CFLAGS)

check_dir:  
	if [ ! -d "$(DIR1)" ]; then \
		mkdir bin; \
	fi

clean:
	rm -f filesystem *.o

clean_obj_files:
	rm -f *.o

run:
	./bin/filesystem