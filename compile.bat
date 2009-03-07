@echo off

(cd src\contrib\cppirc && mingw32-make.exe -f Makefile.win libircinterface.a libsstring.a && cd ..\.. && mingw32-make.exe efirc.exe) 2> build.log

rem ERRORLEVEL abfragen und ggf.
rem ans Ende der Datei springen
rem und den Programmaufruf
rem auslassen
if ERRORLEVEL 1 goto error

cd ..\bin
copy ..\src\efirc.exe efirc.exe > NUL

strip --strip-all efirc.exe

set /p input="run efirc.exe? (`n' to abort) "
if NOT "%input%" == "n" start efirc.exe
exit 0

rem Fenster offen halten, um
rem moegliche Fehler zu analysieren
:error
echo error (see build.log for details)
pause
