#include "simlib.h"

///time to lift the skier uphill
const int way_up =  20;

///first lift
class Facility Lift("Lift 1");

///skier
class Skier : public Process {
    double max_time = 1000;
    void Behavior(){
       Print("Skier arrived.\n");
       Print(Time);
       Seize(Lift);
       Wait(way_up);
       Release(Lift);
       Print("Skier left the lift.\n");
       Print(Time);
    }
};

class Generator : public Event{
    int skier_cnt = 0;
    void Behavior(){
        Print(skier_cnt);
        (new Skier)->Activate();
        skier_cnt ++;
        Activate(Time + Exponential(50));
    }
};

int main(int argc, char* argv[]){
    Print("Project IMS 2023");
    Init(0, 1000);
    (new Generator)->Activate();
    Run();
    Print("Simulation done.");
    Lift.Output();

    return 0;
}
