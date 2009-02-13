OBJS= io.o irccommands.o ircsocket.o tcpip.o
RANLIB= ranlib

libircsocket.a: $(OBJS)
	$(AR) $(ARFLAGS) $(@) $(^)
	$(RANLIB) $(@)

libsstring.a: sstring.o
	$(AR) $(ARFLAGS) $(@) $(^)
	$(RANLIB) $(@)

$(OBJS): ircsocket.h
sstring.o: sstring.h

clean:
	-rm $(OBJS) libircsocket.a libsstring.a cppirc
