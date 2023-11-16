
////////////////////////////////////////////////////////////////////////////
// Model multiexp.cc              SIMLIB/C++
//
// Multiple experiments with various parameters
//

#include "simlib.h"

const double ENDTime = 2000;      // time of simulation

Facility  Box("Box");
Histogram Table("Table",0,25,20);

class Customer : public Process {
    double Prichod;                 // atribute of each customer
    void Behavior() {               // --- behavoir specification ---
        Prichod = Time;               // incoming time
        Seize(Box);                   // start of service
        Wait(10);                     // time of service
        Release(Box);                 // end of service
        Table(Time-Prichod);          // waiting and service time
    }
public:
    Customer() { Activate(); }
};

class Generator : public Event {  // model of system's input
    double dt;
    void Behavior() {               // --- behavior specification ---
        new Customer;                    // new customer
        Activate(Time+Exponential(dt));  //
    }
public:
    Generator(double d) : dt(d) { Activate(); }
};

void Sample() {
    if(Time>0) Print(" %g", Table.stat.MeanValue());
}
Sampler s(Sample,500);

int main() {
    SetOutput("multiexp.dat");    // output redirection
    _Print("# MULTIEXP - multiple experiments example\n");
    for(int i=1; i<=20; i++)  {
        Print("# Experiment#%d (čas=%g) \n", i, ENDTime);
        Init(0,ENDTime);            // time of simulation is 0..ENDTime
        Box.Clear();                // initialization of all objects!
        Table.Clear();
        double interval = i;
        new Generator(interval);    // customer generator
        Print("%g ", interval);
        Run();                      // simulation experiment
        Print(" %g\n", Table.stat.MeanValue());
    }
    return 0;
}
