/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

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

