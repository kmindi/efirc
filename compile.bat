@echo off

rem die Werkzeuge zum Bau befinden sich
rem in einem Unterverzeichnis
echo setting env
set PATH=./sys/bin
echo %PATH%

rem make wird fuer das aktuelle Makefile
rem aufgerufen
echo calling make
mingw32-make.exe all
cd bin
start efirc.exe
