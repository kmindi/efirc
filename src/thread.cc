// thread.cc
// implementiert die Klasse Thread

#include <zentrale.h>
#include <thread.h>
DECLARE_APP(Zentrale) //braucht man fuer wxGetApp um auf die funktionen zuzugreifen

//diese Klasse ist mit der Klasse Zentrale befreundet

//Konstruktor erwartet Zeiger auf eine Funktion die Mitglied der Zentrale ist
Thread::Thread(void (Zentrale::*fkt_zum_aufrufen)())
{
   adresse = fkt_zum_aufrufen;
}
Thread::~Thread() { }

void *Thread::Entry()
{
    (wxGetApp().*adresse)(); //Mitgliedsfunktion wird aufgerufen
}

