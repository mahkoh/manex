include config.mk

CFLAGS = -O3
CFLAGS += -DDOCDIR=\"$(DOCDIR)\"

DOCS = $(shell find pre/ -type f | sed 's/\.mx//' | sed 's/^pre/docs/')
BINS = gen manex

all: manex $(DOCS)

$(DOCS): docs/%: pre/%.mx gen
	@echo GEN $*
	@mkdir -p $(@D)
	@./gen $< > $@

$(BINS): %: %.c config.mk
	$(CC) $(CFLAGS) -o $@ $<

install: all
	install -m755 manex $(BINDIR)
	mkdir -p $(DOCDIR)
	cp -r docs/* $(DOCDIR)

clean:
	rm -rf docs gen manex

.phony: all clean install
