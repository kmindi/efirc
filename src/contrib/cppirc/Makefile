OBJS= io.o irccommands.o ircsocket.o tcpip.o
RANLIB= ranlib

libircinterface.a: $(OBJS)
	$(AR) $(ARFLAGS) $(@) $(^)
	$(RANLIB) $(@)

libsstring.a: sstring.o
	$(AR) $(ARFLAGS) $(@) $(^)
	$(RANLIB) $(@)

$(OBJS): ircsocket.h
sstring.o: sstring.h

clean:
	-rm $(OBJS) libircinterface.a libsstring.a cppirc
