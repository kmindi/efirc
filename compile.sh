# set +v

(cd src/contrib/cppirc && make -f GNUmakefile.linux libircinterface.a libsstring.a && \
cd ../.. && make -f Makefile.linux efirc) 2> build.log

# ERRORLEVEL abfragen und ggf.
# ans Ende der Datei springen
# und den Programmaufruf
# auslassen
if [ $? -eq 0 ]; then
	cd bin
	cp ../src/efirc efirc

	strip --strip-all efirc

	read -p "run efirc? (\`n' to abort) " input
	if [ "$input" != "n" ]; then (./efirc &); fi
	exit 0
fi

# Fenster offen halten, um
# moegliche Fehler zu analysieren
echo "error (see build.log for details)"
exit 1
