# set +v

(make -C src/contrib/cppirc libircinterface.a libsstring.a && make -C src -f Makefile.linux efirc.exe) 2> build.log

# ERRORLEVEL abfragen und ggf.
# ans Ende der Datei springen
# und den Programmaufruf
# auslassen
if [ $? -ne 1 ]; then
	cd bin
	cp ../src/efirc.exe efirc.exe > /dev/null

	strip --strip-all efirc.exe

	read -p "run efirc.exe? ([y]es or any key) " input
	if [ "$input" = "y" ]; then (./efirc.exe &); fi
	exit
fi

# Fenster offen halten, um
# moegliche Fehler zu analysieren
echo "error (see build.log for details)"
