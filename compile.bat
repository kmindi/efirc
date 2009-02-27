@echo off

(mingw32-make.exe -C src/contrib/cppirc libircinterface.a libsstring.a && mingw32-make.exe -C src efirc.exe) 2> build.log

rem ERRORLEVEL abfragen und ggf.
rem ans Ende der Datei springen
rem und den Programmaufruf
rem auslassen
if ERRORLEVEL 1 goto error

cd bin
copy ..\src\efirc.exe efirc.exe > NUL

strip --strip-all efirc.exe

set /p input="run efirc.exe? ([y]es or any key) "
if "%input%" == "y" start efirc.exe
exit

rem Fenster offen halten, um
rem moegliche Fehler zu analysieren
:error
echo error (see build.log for details)
pause
