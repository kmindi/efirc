CPPIRCDIR= contrib/cppirc
OBJS= irc.o ui.o thread.o config.o irc_event_functions.o
CXXFLAGS+= -L${CPPIRCDIR} -Iinclude -I${CPPIRCDIR}
LDLIBS= -lircinterface -lsstring -mwindows -lwxmsw28 -lwsock32 -luuid -lcomctl32 -lole32 -loleaut32

efirc.exe: zentrale.cc $(OBJS) ${CPPIRCDIR}/libircinterface.a ${CPPIRCDIR}/libsstring.a
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(@) $(<) $(LDLIBS)

irc.o irc_event_functions.o: ${CPPIRCDIR}/ircinterface.h

clean:
	$(RM) $(OBJS) efirc.exe
