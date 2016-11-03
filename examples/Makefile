CPPFLAGS += -I../include

CFLAGS += -std=c11
CFLAGS += -Wall
CFLAGS += -MP -MD

.PHONY: all
all: stuff.o
	@

.PHONY: clean
clean:
	$(RM) *.d *.o

-include *.d
