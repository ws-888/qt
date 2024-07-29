LDFLAGS+=-lpthread
LDFLAGS+=-lsqlite3
server : server.o
	gcc -o $@ $^ $(LDFLAGS)
clean:
	rm *.o server
