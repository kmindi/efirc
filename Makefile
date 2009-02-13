SRCDIR=src
BINDIR=bin

efirc:
	$(MAKE_COMMAND) -C $(SRCDIR)/contrib/cppirc libircsocket.a libsstring.a
	$(MAKE_COMMAND) -C $(SRCDIR)
