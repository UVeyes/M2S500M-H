#Toolchain
CC := gcc

#Additional flags
all:
	gcc -o WiFi_Stream_ARM32 WiFi_Stream.cpp -L. libdvp.so libhzd.so -lpthread -Wl,-rpath=.

clean:
	rm *.o 

