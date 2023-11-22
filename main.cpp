#include "simlib.h"

///time to lift the skier uphill
const int way_up_marta1 =  20;
const int way_up_marta2 =  30;
const int way_up_kotva =  25;
const int departure =  10;


///first lift

Store marta1(2);
Store marta2(4);
Store kotva(2);

enum lift {
    MARTA1,    // 0
    MARTA2,  // 1
    KOTVA    // 2
};

bool lift_ready = false;
bool ok = true;
bool open = false;

class Openning_hours: public Event{
  void Behavior(){
      open = true;
      Activate(3000);
      open = false;
      Cancel();
  }
};
///skier
class Skier : public Process {
    // double max_time;
    // bool pauzicka
    // ski vs board

    int jizdy = 3;
    lift current_lift;

    void choose_start(){
        int val = Random();
        if(val < 0.33){
            Print("Entered MARTA1");
            current_lift = MARTA1;
        }
        else if(val < 0.66){
            Print("Entered MARTA2");
            current_lift = MARTA2;
        }
        else{
            Print("Entered KOTVA");
            current_lift = KOTVA;
        }
    }

    void ride_down(){
        if(current_lift == MARTA1 || current_lift == KOTVA){// vyjel uplne nahoru
          double val = Random();
            if(val < 0.33){
                Wait(108);
                current_lift = KOTVA;
            }
            else if(val < 0.66){
                Wait(108);
                current_lift = MARTA2;
            }
            else{ //slunecna
                Wait(36);
                val = Random();
                if (val < 0.5){
                    Wait(108); // sjizdi marta 2
                    current_lift = MARTA2;

                }else{
                    Wait(168); //prejezd marta 2 -> 1
                    current_lift = MARTA1;
                }

            }

        }else{ //vyjel do pulky pres marta 2
            double val = Random();
            if (val < 0.5){
                Wait(108); // sjizdi marta 2
                current_lift = MARTA2;
            }else{
                Wait(168); //prejezd marta 2 -> 1
                current_lift = MARTA1;
            }

        }

    }
    
    void ride_up(){
        
        if(current_lift == MARTA1){
            Enter(marta1, 1);
            Wait(departure);
            Leave(marta1, 1);
            Wait(way_up_marta1);
        }
        else if (current_lift == MARTA2){
            Enter(marta2, 1);
            Wait(departure);
            Leave(marta2, 1);
            Wait(way_up_marta2);
        }
        else {
            Enter(kotva, 1);
            Wait(departure);
            Leave(kotva, 1);
            Wait(way_up_kotva);

        }
    }

    void Behavior() override{
       Print("Skier generated.\n");
       Print(Time);
       
       choose_start();
       while(jizdy > 0){
               ride_up();
               ride_down();
               jizdy --;
       }
       Print("Skier leaving the system.\n");
       Print(Time);
    }
};

class Generator : public Event{
    int skier_cnt = 0;
    // nocni rezim
    //dennin rezim
    void Behavior() override{
        Print(skier_cnt);
        (new Skier)->Activate();
        skier_cnt ++;
        Activate(Time + Exponential(5));
    }
};

int main(int argc, char* argv[]){

    Print("Project IMS 2023");
    Init(0, 1000);
    (new Openning_hours)->Activate();

    (new Generator)->Activate();

    Run();
    Print("Simulation done.");
    Print("Marta 1");
    marta1.Output();
    Print("Marta 2");
    marta2.Output();
    Print("kotva");
    kotva.Output();


    return 0;
}
