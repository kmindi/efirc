@echo off

rem die Werkzeuge zum Bau befinden sich
rem in einem Unterverzeichnis
echo setting env
set PATH=%PATH%;sys\bin
echo PATH=%PATH%

rem make wird fuer das aktuelle Makefile
rem aufgerufen
echo.
echo calling make
set WINDOWS=1
mingw32-make all

rem ERRORLEVEL abfragen und ggf.
rem ans Ende der Datei springen
rem und den Programmaufruf
rem auslassen
if ERRORLEVEL 1 goto error

rem keine Fehler, beenden und
rem efirc.exe in bin aufrufen
echo.
echo calling efirc
cd bin
efirc
exit 0

rem Fenster offen halten, um
rem moegliche Fehler zu analysieren
:error
pause
