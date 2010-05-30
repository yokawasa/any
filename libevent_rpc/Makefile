CC = gcc
CPPC = g++
CFLAGS = -I/usr/include -I/usr/local/include -Wall -g
LIBS = -L/usr/local/lib -levent

.SUFFIXES: .cpp .c .o

all: simple_rpc_client simple_rpc_server

simple_rpc_client: simple_rpc_client.o simple.gen.o
	$(CPPC) $(LIBS) -o $@ $?

simple_rpc_server: simple_rpc_server.o simple.gen.o
	$(CPPC) $(LIBS) -o $@ $?

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CPPC) $(CFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) *.o simple_rpc_client simple_rpc_server
