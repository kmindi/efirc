Bauanleitung

1) Herunterladen und Installieren von TortoiseSVN[1]
2) Erstellen einer Arbeitskopie von wxWidgets[2] (und efirc[3])
3) Herunterladen und Entpacken von Mingw[4] (schließlich Hinzufügen von bin zu PATH)
4) Bauen von wxWidgets durch Ausführen von 
   "mingw32-make -f makefile.gcc"
   in build\msw (bei Bedarf mit anderen Optionen, siehe config.gcc)
5) Herunterladen von wx-config für Windows[5] (muss per PATH verfügbar sein)
6) Setzen der Umgebungsvariablen WXWIN auf den Pfad der Arbeitskopie von wxWidgets (zB.: C:\wxWidgets)
7) Setzen der Umgebungsvariablen WXCFG auf den Pfad der Baukonfiguration (in der Regel gcc_lib\mswu, liegt unter lib)
7) Bauen von efirc durch Aufruf von compile.bat in trunk\src 

[1] http://tortoisesvn.net/downloads
    - TortoiseSVN-1.6.2.16344-win32-svn-1.6.2.msi

[2] https://svn.wxwidgets.org/svn/wx/wxWidgets/trunk

[3] https://efirc.svn.sourceforge.net/svnroot/efirc/trunk

[4] http://sourceforge.net/project/showfiles.php?group_id=2435
    - binutils-2.19.1-mingw32-bin.tar.gz
    - gcc-core-3.4.5-20060117-3.tar.gz
    - gcc-g++-3.4.5-20060117-3.tar.gz
    - mingw32-make-3.81-20080326-2.tar.gz
    - mingwrt-3.15.2-mingw32-dev.tar.gz
    - w32api-3.13-mingw32-dev.tar.gz

[5] http://wxconfig.googlepages.com/
    - wx-config.exe
