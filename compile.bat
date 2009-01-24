@echo off

rem make wird fuer das aktuelle Makefile
rem aufgerufen
echo.
echo ...calling make...
set WINDOWS=1
mingw32-make all

rem echo ...Stripping...
rem strip --strip-all bin\test.exe

rem echo ...c0mprim!sing...
rem upx --best bin\test.exe

rem ERRORLEVEL abfragen und ggf.
rem ans Ende der Datei springen
rem und den Programmaufruf
rem auslassen
if ERRORLEVEL 1 goto error

rem keine Fehler, beenden und
rem efirc.exe in bin aufrufen
echo.
echo ...starting...
cd bin
efirc
rem exit 0

rem Fenster offen halten, um
rem moegliche Fehler zu analysieren
:error
pause
