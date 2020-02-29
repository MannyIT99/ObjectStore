CC = gcc
AR = ar
CFLAGS += -std=gnu99 -g
ARFLAGS = rvs
LDFLAGS = -L.
LIBS = -lpthread

.PHONY: all clean test
.SUFFIXES: .c .h .a

TARGETS = server \
					client

%: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(TARGETS)

server: server.o clientconnessi.a
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS) clientconnessi.a

client: client.o accesso.a
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) accesso.a

accesso.a: accesso.o accesso.h
	$(AR) $(ARFLAGS) $@ $<

clientconnessi.a: clientconnessi.o clientconnessi.h
	$(AR) $(ARFLAGS) $@ $<

clean:
	rm -f $(TARGETS) client.o server.o clientconnessi.a clientconnessi.o accesso.o accesso.a

test:
	./server &
	bash script.sh 2>&1 | tee testout.log
