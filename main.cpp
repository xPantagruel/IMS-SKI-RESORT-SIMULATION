#include "simlib.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <string.h>

/// time to lift the skier uphill
const int way_up_marta1 = 300;
const int way_up_marta2 = 180;
const int way_up_kotva = 300;
const int way_up_poma = 180;

//frequency of lift
const int departure_kotva = 12;
const int departure_poma = 12;
const int departure_marta1 = 12;
const int departure_marta2 = 6;

//ride down
const int poma_marta1 = 84;
const int poma_marta2 = 78;
const int marta2_marta1 = 168;
const int marta1_marta1 = 108;
const int marta2_marta2 = 108;
const int slunecna = 36;


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
               Wait(Normal(marta1_marta1, 10));
               double val2 = Random();
               if(val2 > 0.5){
                   current_lift = MARTA1;
               }else{
                   current_lift = KOTVA;
               }

            }
            else
            { // slunecna
                Wait(Normal(slunecna, 10));
                val = Random();
                if (val < 0.9)
                {
                    Wait(Normal(marta2_marta2, 10)); // sjizdi marta 2
                    double val2 = Random();
                    if(val2 < 0.9){
                        current_lift = MARTA2;
                    }else{
                        current_lift = POMA;
                    }

                }
                else
                {
                    Wait(Normal(marta2_marta1, 10)); // prejezd marta 2 -> 1
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
            if (val < 0.7)
            {
                Wait(Normal(poma_marta2, 10)); // sjizdi marta 2
                double val2 = Random();
                if(val2 < 0.9){
                    current_lift = MARTA2;
                }else{
                    current_lift = POMA;
                }
            }
            else
            {
                Wait(Normal(poma_marta1, 10)); // prejezd marta 2 -> 1
                double val2 = Random();
                if(val2 < 0.5){
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
                Wait(Normal(marta2_marta2, 10)); // sjizdi marta 2
                double val2 = Random();
                if(val2 < 0.9){
                    current_lift = MARTA2;
                }else{
                    current_lift = POMA;
                }
            }
            else
            {
                Wait(Normal(marta2_marta1, 10)); // prejezd marta 2 -> 1
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
            Wait(departure_marta1);
            Leave(marta1, 1);
            Wait(way_up_marta1);
        }
        else if (current_lift == MARTA2)
        {
            Enter(marta2, 1);
            Wait(departure_marta2);
            Leave(marta2, 1);
            Wait(way_up_marta2);
        }
        else if (current_lift == POMA){
            Seize(poma);
            Wait(departure_poma);
            Release(poma);
            Wait(way_up_poma);
        }
        else{
            Enter(kotva, 1);
            Wait(departure_kotva);
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
class GeneratorDay : public Event
{

    double current_time = 0.0;

    void Behavior() override
    {
        current_time = Time;

        if (!open)
        {
            Cancel();
            Print("Closed");
            //Print(skier_cnt);
        }
        else
        {
            //Print(skier_cnt);
            (new Skier)->Activate();
            skier_cnt++;

            if (current_time < 10800)
            {                                    // 8:30 - 11:00 (ULTRA RUSH)
                Activate(Time + Exponential(8)); // Adjusted rate for ULTRA RUSH hours
            }
            else if (current_time < 13200)
            {                                     // 11:00 - 12:00 (MEDIUM RUSH)
                Activate(Time + Exponential(15)); // Adjusted rate for MEDIUM RUSH hours
            }
            else if (current_time < 18000)
            {                                     // 12:00 - 13:00 (LOW RUSH)
                Activate(Time + Exponential(20)); // Adjusted rate for LOW RUSH hours
            }
            else if (current_time < 21600)
            {                                     // 13:00 - 15:00 (HIGH RUSH)
                Activate(Time + Exponential(15)); // Adjusted rate for HIGH RUSH hours
            }
            else if (current_time < 27000)
            {                                     // 15:00 - 16:00 (LOW RUSH)
                Activate(Time + Exponential(10)); // Adjusted rate for LOW RUSH hours
            }
            else
            {
                Cancel();
            }
        }
    }
};

class GeneratorNight : public Event
{

    double current_time = 0.0;

    void Behavior() override
    {
        current_time = Time;

        if (!open)
        {
            Cancel();
            Print("Closed");
            //Print(skier_cnt);
        }
        else
        {
            //Print(skier_cnt);
            (new Skier)->Activate();
            skier_cnt++;

            // 18:00 - 21:00
            if (current_time < 3600) // 18:00 - 19:00
            {
                Activate(Time + Exponential(8)); // HIGH RUSH
            }
            else if (current_time < 7200) // 19:00 - 20:00
            {
                Activate(Time + Exponential(12)); // MEDIUM RUSH
            }
            else if (current_time < 10800) // 20:00 - 21:00
            {
                Activate(Time + Exponential(25)); // LOW RUSH
            }
        }
    }
};

void print_help()
{
    Print("Usage: ./ims [day|night]\n");
}

bool parse_args(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "day") == 0)
        {
            return true;
        }
        else if (strcmp(argv[1], "night") == 0)
        {
            return false;
        }
        else
        {
            print_help();
            exit(0);
        }
    }
    return true;
}


int main(int argc, char *argv[])
{
    bool day = parse_args(argc, argv); // by default day if no args

    Print("Project IMS 2023");
    Init(0, 40000);
    double day_time = 27000;
    double night_time = 10800;

    if (day)
    { // day
        (new Open_hours(day_time))->Activate();
        (new GeneratorDay)->Activate();
    }
    else
    { // night
        (new Open_hours(night_time))->Activate();
        (new GeneratorNight)->Activate();
    }
    // (new Open_hours(day_time))->Activate();
    // (new Generator)->Activate();

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