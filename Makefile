EXEC_FILES = $(patsubst src/%.c,bin/%,$(wildcard src/*.c))

all: $(EXEC_FILES)

debug: python
	cd src; $(MAKE) debug

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
