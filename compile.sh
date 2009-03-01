# set +v

(make -C src/contrib/cppirc -f Makefile.linux libircinterface.a libsstring.a && make -C src -f Makefile.linux efirc) 2> build.log

# ERRORLEVEL abfragen und ggf.
# ans Ende der Datei springen
# und den Programmaufruf
# auslassen
if [ $? -ne 1 ]; then
	cd bin
	cp ../src/efirc efirc > /dev/null

	strip --strip-all efirc

	read -p "run efirc? ([y]es or any key) " input
	if [ "$input" = "y" ]; then (./efirc &); fi
	exit
fi

# Fenster offen halten, um
# moegliche Fehler zu analysieren
echo "error (see build.log for details)"
