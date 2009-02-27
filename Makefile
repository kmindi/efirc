SRCDIR=src
BINDIR=bin

efirc:
	$(MAKE_COMMAND) -C $(SRCDIR)/contrib/cppirc libircinterface.a libsstring.a
	$(MAKE_COMMAND) -C $(SRCDIR)
