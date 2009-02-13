@echo off

rem make wird fuer das aktuelle Makefile
rem aufgerufen
echo.
echo ...calling make...
mingw32-make.exe 2> build.log

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
move ..\src\efirc.exe efirc.exe
efirc.exe
exit %ERRORLEVEL%

rem Fenster offen halten, um
rem moegliche Fehler zu analysieren
:error
echo.
echo ERRORLEVEL %ERRORLEVEL% (see build.log for details)
pause
