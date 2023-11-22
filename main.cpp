////////////////////////////////////////////////////////////////////////////
// model2-timeout.cc              SIMLIB/C++
//
// Ukázkový model s netrpělivými požadavky
//

#include "simlib.h"

const int TIMEOUT = 20;
const int SERVICE = 10;

// deklarace globálních objektů
Facility  Box("Linka");
Histogram Tabulka("Tabulka", 0, 1, SERVICE+TIMEOUT+1);
long ncount = 0;

class Timeout : public Event {
    Process *ptr;        // který proces
public:
    Timeout(double t, Process *p): ptr(p) {
        Activate(Time+t);  // kdy vyprší timeout
    }
    void Behavior() {
        ptr->Out();        // vyjmout z fronty
        delete ptr;        // likvidace
        ncount++;          // počitadlo
        Cancel();          // konec události (SIMLIB BUG)
    }
};

class Zakaznik : public Process { // třída zákazníků
    double Prichod;                 // atribut každého zákazníka
    void Behavior() {               // --- popis chování zákazníka ---
        Prichod = Time;               // čas příchodu zákazníka
        Event *timeout = new Timeout(TIMEOUT,this); // nastavit timeout
        Seize(Box);                   // obsazení zařízení Box
        delete timeout; // zrušit neaktivovaný timeout
        Wait(SERVICE);                     // obsluha
        Release(Box);                 // uvolnění
        Tabulka(Time-Prichod);        // doba obsluhy a čekání
    }
};

class Generator : public Event {  // generátor zákazníků
    void Behavior() {               // --- popis chování generátoru ---
        (new Zakaznik)->Activate();   // nový zákazník, aktivace v čase Time
        Activate(Time+Exponential(1e3/150));  // interval mezi příchody
    }
};

int main() {                 // popis experimentu s modelem
//DebugON();
    Print("model2-timeout -- příklad SIMLIB/C++\n");
    SetOutput("model2-timeout.out");
    Init(0,1000);              // inicializace experimentu, čas bude 0..1000
    (new Generator)->Activate(); // generátor zákazníků, jeho aktivace
    Run();                     // simulace
    Print("Počet netrpělivých zákazníků: %d \n", ncount);
    Box.Output();              // tisk výsledků
    Tabulka.Output();
    return 0;
}

// konec