# set +v

(make -C src/contrib/cppirc -f Makefile.linux libircinterface.a libsstring.a && make -C src -f Makefile.linux efirc) 2> build.log

# ERRORLEVEL abfragen und ggf.
# ans Ende der Datei springen
# und den Programmaufruf
# auslassen
if [ $? -ne 2 ]; then
	cd bin
	cp ../src/efirc efirc

	strip --strip-all efirc

	read -p "run efirc? (`n' to abort) " input
	if [ "$input" != "n" ]; then (./efirc &); fi
	exit
fi

# Fenster offen halten, um
# moegliche Fehler zu analysieren
echo "error (see build.log for details)"
