CC=gcc
CFLAGS=-Wall -g

SRCDIR=src
BINDIR=bin

help:	Makefile
	@sed -n 's/^##[ ]*/\n/p' $< | sed 's/: /:\n\t/g; 1d'

## bin/connect4: compile connect4
$(BINDIR)/connect4:	$(SRCDIR)/connect4.c
	make clean
	mkdir $(BINDIR)
	${CC} ${CFLAGS} $^ -lncurses -o $(BINDIR)/connect4

## install: install connect4 binary
install:	$(BINDIR)/connect4
	[ -d /usr/local/bin ] || sudo mkdir -p /usr/local/bin
	cd $(BINDIR); find . -type f -exec sudo cp -v {} /usr/local/bin/ \;

## uninstall: uninstall connect4 binary
uninstall:	$(BINDIR)/connect4
	cd $(BINDIR); find . -type f -exec sudo rm -v /usr/local/bin/{} \;

## clean: remove bin dir (and everything inside)
clean:
	rm -rf $(BINDIR) &> /dev/null || true
