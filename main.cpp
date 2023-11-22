#include "simlib.h"

///time to lift the skier uphill
const int way_up =  20;

///first lift
bool Marta1_seat_1_free = true;
class Facility Marta1_seat_1("marta 1 seat 1");

bool Marta1_seat_2_free = true;
class Facility Marta1_seat_2("marta 1 seat 2");


class Facility Marta2("marta 2");
class Facility Kotva("kotva");



///skier
class Skier : public Process {
    // double max_time;
    // bool pauzicka
    // ski vs board
    int current_marta1 = 0;
    void seize_marta1(){
        if(Marta1_seat_1_free){
            current_marta1 = 1;
            Marta1_seat_1_free = false;
            Seize(Marta1_seat_1);
        }
        else{
            current_marta1 = 2;
            Marta1_seat_2_free = false;
            Seize(Marta1_seat_2);
        }
    }
    void release_marta1(){
        if(current_marta1 == 1){
            current_marta1 = 0;
            Release(Marta1_seat_1);
            Marta1_seat_1_free = true;


        } else{
            current_marta1 = 0;
            Seize(Marta1_seat_2);
            Marta1_seat_2_free = true;

        }
    }
    void Behavior(){
       Print("Skier arrived.\n");
       Print(Time);
       seize_marta1();
       Wait(way_up);
       release_marta1();
       //random rozhodovani
       Print("Skier left the lift.\n");
       Print(Time);
    }
};

class Generator : public Event{
    int skier_cnt = 0;
    // nocni rezim
    //dennin rezim
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
    Marta1_seat_1.Output();
    Marta1_seat_2.Output();

    return 0;
}
