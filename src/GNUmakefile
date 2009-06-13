CPPIRCDIR:= contrib\cppirc
OBJS:= irc.o ui.o thread.o config.o irc_event_functions.o
WXCXXFLAGS:= $(shell wx-config --cxxflags)
CXXFLAGS+= $(WXCXXFLAGS) -L$(CPPIRCDIR) -Iinclude -I$(CPPIRCDIR)
LDLIBS:= $(shell wx-config --libs) -lwxmsw29u_aui -lwxmsw29u_core -lircinterface -lsstring -mwindows -luuid -lcomctl32 -lole32 -loleaut32 -lwsock32

efirc.exe: zentrale.cc $(OBJS) $(CPPIRCDIR)\libircinterface.a $(CPPIRCDIR)\libsstring.a
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(@) $(<) $(LDLIBS)

irc.o irc_event_functions.o: $(CPPIRCDIR)\ircinterface.h

clean:
	$(RM) $(OBJS) efirc.exe
