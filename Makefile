PREFIX     = /usr
EXEC_FILES = $(patsubst src/%.c,bin/%,$(wildcard src/*.c))

all: $(EXEC_FILES)

debug:
	cd src; $(MAKE) debug

install:
	install -d $(PREFIX)/bin
	install -d $(PREFIX)/Resources
	install --mode=644 Resources/Dependencies $(PREFIX)/Resources
	install --mode=755 $(wildcard bin/*) $(PREFIX)/bin

clean:
	rm -rf Resources/FileHash*
	find * -path "*~" -or -path "*/.\#*" -or -path "*.bak" | xargs rm -f
	cd src && $(MAKE) clean
	rm -f $(EXEC_FILES)

$(EXEC_FILES): bin/%: src/%
	cp -af $< $@
	chmod a+x $@

src/%: src/%.c
	$(MAKE) -C src

.PHONY: all debug clean
