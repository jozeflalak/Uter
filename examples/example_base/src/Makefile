
#GCC_FLAGS = -g -O
GCC_FLAGS = -g

all:
	gcc $(GCC_FLAGS) -c -fPIC so_debug.c -o so_debug.o
	gcc -shared so_debug.o -o libdyndebug.so
	
	gcc -c -fPIC so_release.c -o so_release.o
	gcc -shared so_release.o -o libdynrelease.so

	gcc $(GCC_FLAGS) -c main.c -o main.o
	gcc $(GCC_FLAGS) -c useful.c -o useful.o
	gcc $(GCC_FLAGS) -c file_1.c -o file_1.o
	gcc $(GCC_FLAGS) -c file_2.c -o file_2.o
	gcc $(GCC_FLAGS) -c file_empty.c -o file_empty.o
	gcc main.o useful.o  file_1.o file_2.o file_empty.o     -L.  -Wl,-rpath,'$$ORIGIN/' -ldyndebug -ldynrelease    -ldl  -lpthread -o test.exe

clean:
	rm -rf *.o
	rm -rf *.so
	rm -rf *.exe

