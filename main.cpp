#include "simlib.h"
#include <vector>
#include <utility>
#include <algorithm>

/// time to lift the skier uphill
const int way_up_marta1 = 300;
const int way_up_marta2 = 180;
const int way_up_kotva = 300;
const int way_up_poma = 180;

const int departure = 10; //todo podle frekvence vleku

const int pause_duration  = 300; // kolik si da cca pauzu, 5 min

const double day_time = 27000;
/// first lift

Store marta1(2);
Store marta2(4);
Store kotva(2);

Facility poma;

enum lift
{
    MARTA1, // 0
    MARTA2, // 1
    KOTVA,   // 2
    POMA//3
};

std::vector<std::pair<double, double>> skier_stats;

bool lift_ready = false;
bool ok = true;
bool open = false;

class Open_hours : public Process
{
private:
    double open_time;

public:
    // Constructor
    Open_hours(double openTime) : open_time(openTime) {}

    void Behavior() override
    {
        open = true;
        Wait(open_time); // Wait for the specified duration
        open = false;
        Cancel(); // Cancel the process
    }
};
/// skier
class Skier : public Process
{
    // double max_time;
    // bool pauzicka
    // ski vs board

    int jizdy = 10;
    lift current_lift;
    double time_spent_skiing = 0;
    double time_spent_other = 0;
    double start_time;
    double pause;
    int pause_cnt;

    void choose_start()
    {
        double val = Random();
        if (val < 0.25)
        {
            current_lift = MARTA1;
        }
        else if (val < 0.5)
        {
            current_lift = MARTA2;

        }else if (val < 0.75){

             current_lift = POMA;
        }
        else
        {
            current_lift = KOTVA;
        }
    }

    void check_pause(){

        if(Time > this->start_time + pause*pause_cnt){
            Wait(Exponential(pause_duration));
            pause_cnt++;
        }

    }

    void ride_down()
    {
        if (current_lift == MARTA1 || current_lift == KOTVA)
        { // vyjel uplne nahoru
            double val = Random();

            if (val < 0.5)
            {
               Wait(108);
               double val2 = Random();
               if(val2 > 0.5){
                   current_lift = MARTA1;
               }else{
                   current_lift = KOTVA;
               }

            }
            else
            { // slunecna
                Wait(36);
                val = Random();
                if (val < 0.7)
                {
                    Wait(108); // sjizdi marta 2
                    double val2 = Random();
                    if(val2 > 0.5){
                        current_lift = MARTA2;
                    }else{
                        current_lift = POMA;
                    }

                }
                else
                {
                    Wait(168); // prejezd marta 2 -> 1
                    double val2 = Random();
                    if(val2 < 0.5){
                        current_lift = MARTA1;
                    }else{
                        current_lift = KOTVA;
                    }
                }
            }
        }
        else if(current_lift == POMA){
            // vyjel kotvou podel marta2 bokem
            double val = Random();
            if (val < 0.7) //TODO change %
            {
                Wait(108); // sjizdi marta 2 TODO change time
                double val2 = Random();
                if(val2 > 0.5){ //todo
                    current_lift = MARTA2;
                }else{
                    current_lift = POMA;
                }
            }
            else
            {
                Wait(168); // prejezd marta 2 -> 1 TODO change time
                double val2 = Random();
                if(val2 < 0.5){ //TODO change %
                    current_lift = MARTA1;
                }else{
                    current_lift = KOTVA;
                }
            }
        }
        else
        { // vyjel do pulky pres marta 2
            double val = Random();
            if (val < 0.7)
            {
                Wait(108); // sjizdi marta 2
                double val2 = Random();
                if(val2 > 0.5){ //todo
                    current_lift = MARTA2;
                }else{
                    current_lift = POMA;
                }
            }
            else
            {
                Wait(168); // prejezd marta 2 -> 1
                double val2 = Random();
                if(val2 < 0.5){
                    current_lift = MARTA1;
                }else{
                    current_lift = KOTVA;
                }
            }
        }
        check_pause();
    }

    void ride_up()
    {

        if (current_lift == MARTA1)
        {
            Enter(marta1, 1);
            Wait(departure);
            Leave(marta1, 1);
            Wait(way_up_marta1);
        }
        else if (current_lift == MARTA2)
        {
            Enter(marta2, 1);
            Wait(departure);
            Leave(marta2, 1);
            Wait(way_up_marta2);
        }
        else if (current_lift == POMA){
            Seize(poma);
            Wait(departure);
            Release(poma);
            Wait(way_up_poma);
        }
        else{
            Enter(kotva, 1);
            Wait(departure);
            Leave(kotva, 1);
            Wait(way_up_kotva);
        }
    }

    void Behavior() override
    {
        start_time = Time;
        pause = Exponential(day_time/4);
        pause_cnt = 1;
        choose_start();
        while (jizdy > 0 && open)
        {
            double tmp = Time;
            ride_up();
            time_spent_other += (Time - tmp);
            tmp = Time;
            ride_down();
            time_spent_skiing += (Time - tmp);
            jizdy--;
        }
        double skiing_ratio;
        if(jizdy == 0){
             skiing_ratio = time_spent_skiing/(time_spent_skiing+time_spent_other)*100;
        }else{
             skiing_ratio =0;
        }

        skier_stats.emplace_back(start_time, skiing_ratio);
    }
};
int skier_cnt = 0;
class Generator : public Event
{

    // todo night openning hours
    // todo day openning hours
    int exp = 0;
    int cnt = 0;

    double get_interval(){
        cnt++;
        if(cnt < 100){ // first 100 visitors
            return Exponential(1);
        }else if(cnt < 1000){
            return Exponential(5);
        }
        else if(cnt < 2000) {
            return Exponential(20);
        }else{
            return Exponential(100);
        }

    }

    void Behavior() override
    {
        if (!open)
        {
            Cancel();
        }
        else
        {
            (new Skier)->Activate();
            skier_cnt++;
            Print("Skier generated at ");
            Print(Time);
            Print("\n");
            Activate(Time + get_interval());
        }
    }
};

int main(int argc, char *argv[])
{
    Print("Project IMS 2023\n");
    Init(0, 40000);
    (new Open_hours(day_time))->Activate();
    (new Generator)->Activate();

    Run();
    std::sort(skier_stats.begin(), skier_stats.end(),
              [](const auto& lhs, const auto& rhs) {
                  return lhs.first < rhs.first;
              });
    // Access and print the values in the vector
    for (const auto& pair : skier_stats) {
        Print("Start: ");
        Print(pair.first);
        Print(" Ratio: ");
        Print(pair.second);
        Print("%\n");
    }

    Print("Simulation done.\n");
    Print("Total number of visitors.");
    Print(skier_cnt);
    Print("\n");
    Print("Marta 1");
    marta1.Output();
    Print("Marta 2");
    marta2.Output();
    Print("kotva");
    kotva.Output();
    Print("poma");
    poma.Output();

    return 0;
}