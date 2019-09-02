CC= gcc
LD= ld 

all: client
.c.o:
	@echo " CC $<"
	@$(CC) $<  -c -o $@
client:main.o http_sock.o
	@echo " Generate excutable file: client"
	@$(CC) -o client http_sock.o main.o -lpthread
clean:
	rm -f *.o client 