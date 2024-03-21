gcc -c alloc_consolne.c -o ac.o
windres res.rc -O coff -o res.o
gcc ac.o res.o -o ac.dll -shared

gcc -c main.c -o main.o
gcc -o Injector.exe main.o
del *.o
