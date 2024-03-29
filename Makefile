CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

debug: CFLAGS += -DDEBUG -g
debug: test

tinyc: $(OBJS)
	$(CC) -o tinyc $(OBJS) $(LDFLAGS)

$(OBJS): tinyc.h

test: tinyc
	./test.sh

clean:
	rm -f tinyc *.o *~ tmp*

.PHONY: test clean debug