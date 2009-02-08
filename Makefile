CXX=mingw32-g++
SRCDIR=src
BINDIR=bin

efirc:
	$(MAKE_COMMAND) -C $(SRCDIR)/contrib/cppirc libircsocket.a libsstring.a CXX=$(CXX)
	$(MAKE_COMMAND) -C $(SRCDIR) CXX=$(CXX)
