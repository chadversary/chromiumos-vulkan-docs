-include config.mk

.PHONY: all
all:
	$(MAKE) -C examples all

.PHONY: clean
clean:
	$(MAKE) -C examples clean
