#include "simlib.h"

/// time to lift the skier uphill
const int way_up_marta1 = 300;
const int way_up_marta2 = 180;
const int way_up_kotva = 300;
const int departure = 10;

/// first lift

Store marta1(2);
Store marta2(4);
Store kotva(2);

enum lift
{
    MARTA1, // 0
    MARTA2, // 1
    KOTVA   // 2
};

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

    void choose_start()
    {
        double val = Random();
        if (val < 0.33)
        {
            Print(val);
            Print("Entered MARTA1");
            current_lift = MARTA1;
        }
        else if (val < 0.66)
        {
            Print("Entered MARTA2");
            current_lift = MARTA2;
        }
        else
        {
            Print("Entered KOTVA");
            current_lift = KOTVA;
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
                    current_lift = MARTA2;
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
        else
        { // vyjel do pulky pres marta 2
            double val = Random();
            if (val < 0.7)
            {
                Wait(108); // sjizdi marta 2
                current_lift = MARTA2;
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
        else
        {
            Enter(kotva, 1);
            Wait(departure);
            Leave(kotva, 1);
            Wait(way_up_kotva);
        }
    }

    void Behavior() override
    {
        Print("Skier generated.\n");
        Print(Time);

        choose_start();
        while (jizdy > 0 && open)
        {
            ride_up();
            ride_down();
            jizdy--;
        }
        Print("Skier leaving the system.\n");
        Print(Time);
    }
};

class Generator : public Event
{
    int skier_cnt = 0;
    // todo night openning hours
    // todo day openning hours
    void Behavior() override
    {
        if (!open)
        {
            Cancel();
        }
        else
        {
            Print(skier_cnt);
            (new Skier)->Activate();
            skier_cnt++;
            Activate(Time + Exponential(15));
        }
    }
};

int main(int argc, char *argv[])
{

    Print("Project IMS 2023");
    Init(0, 40000);
    double day_time = 27000;
    (new Open_hours(day_time))->Activate();
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