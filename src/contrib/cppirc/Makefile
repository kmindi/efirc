OBJS= io.o irccommands.o ircsocket.o tcpip.o
LDLIBS= -lircinterface -lpthread

.ifdef DEBUG
CXXFLAGS+= -g -Wall
.endif

.MAIN: libircinterface.a
.PATH.h: $(.OBJDIR)
.PATH.a: $(.OBJDIR)
.INCLUDES: .h
.LIBS: .a

cppirc: libircinterface.a cppirc.cc
	$(CXX) $(CXXFLAGS) -o $(.TARGET) $(.INCLUDES) $(.LIBS) \
	$(.IMPSRC) $(LDLIBS)

libircinterface.a: $(OBJS)
	$(AR) $(ARFLAGS) $(.TARGET) $(.ALLSRC)
	$(RANLIB) $(.TARGET)

libsstring.a: sstring.o
	$(AR) $(ARFLAGS) $(.TARGET) $(.ALLSRC)
	$(RANLIB) $(.TARGET)

$(OBJS): ircinterface.h
sstring.o: sstring.h

clean:
	-rm $(OBJS) sstring.o libircinterface.a libsstring.a cppirc
