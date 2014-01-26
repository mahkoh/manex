include config.mk

CFLAGS = -O3 -std=c99 -D_GNU_SOURCE
CFLAGS += -DDOCDIR=\"$(DOCDIR)\"

DOCS = $(shell find pre/ -type f -iname '*.mx' | sed 's/\.mx$$//' | sed 's/^pre/docs/')
BINS = gen manex

all: $(BINS) $(DOCS)

$(DOCS): docs/%: pre/%.mx gen
	@./gen

$(BINS): %: %.c config.mk
	$(CC) $(CFLAGS) -o $@ $<

install: all
	install -m755 manex $(BINDIR)
	mkdir -p $(DOCDIR)
	cp -r docs/* $(DOCDIR)

clean:
	rm -rf docs $(BINS)

.phony: all clean install
