CXX=mingw32-g++
SRCDIR=src
BINDIR=bin

$(BINDIR)/efirc.exe:
	$(MAKE_COMMAND) -C $(SRCDIR)/contrib/cppirc CXX=$(CXX)
	$(MAKE_COMMAND) -C $(SRCDIR) CXX=$(CXX)
