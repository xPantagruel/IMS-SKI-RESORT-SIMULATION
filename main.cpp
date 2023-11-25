#include "simlib.h"
#include <cstring>

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
                if (val2 > 0.5)
                {
                    current_lift = MARTA1;
                }
                else
                {
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
                    if (val2 < 0.5)
                    {
                        current_lift = MARTA1;
                    }
                    else
                    {
                        current_lift = KOTVA;
                    }
                }
            }
        }
        else
        { // vyjel do pulky pres marta 2
            double val = Random();
            if (val < 0.5)
            {
                Wait(108); // sjizdi marta 2
                current_lift = MARTA2;
            }
            else
            {
                Wait(168); // prejezd marta 2 -> 1
                double val2 = Random();
                if (val2 < 0.5)
                {
                    current_lift = MARTA1;
                }
                else
                {
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

class GeneratorDay : public Event
{
    int skier_cnt = 0;
    bool isMorning = true;
    double current_time = 0.0;

    void Behavior() override
    {
        current_time = Time;

        if (!open)
        {
            Cancel();
            Print("Closed");
            Print(skier_cnt);
        }
        else
        {
            Print(skier_cnt);
            (new Skier)->Activate();
            skier_cnt++;

            if (current_time < 1800) // 8:30 - 9:00 ULTRA RUSH
            {
                Activate(Time + Exponential(6));
            }
            else if (current_time < 5400) // 9:00 - 10:00 HIGH RUSH
            {
                Activate(Time + Exponential(10));
            }
            else if (current_time < 9000) // 10:00 - 11:00 HIGH RUSH
            {
                Activate(Time + Exponential(15));
            }
            else if (current_time < 12600) // 11:00 - 12:00 MEDIUM RUSH
            {
                Activate(Time + Exponential(20));
            }
            else if (current_time < 16200) // 12:00 - 13:00 LOW RUSH
            {
                Activate(Time + Exponential(25));
            }
            else if (current_time < 19800) // 13:00 - 14:00 ULTRA RUSH
            {
                Activate(Time + Exponential(20));
            }
            else if (current_time < 23400) // 14:00 - 15:00 HIGH RUSH
            {
                Activate(Time + Exponential(15));
            }
            else if (current_time < 27000) // 15:00 - 16:00 LOW RUSH
            {
                Activate(Time + Exponential(10));
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
    int skier_cnt = 0;
    double current_time = 0.0;

    void Behavior() override
    {
        current_time = Time;

        if (!open)
        {
            Cancel();
            Print("Closed");
            Print(skier_cnt);
        }
        else
        {
            Print(skier_cnt);
            (new Skier)->Activate();
            skier_cnt++;

            // 18:00 - 21:00
            if (current_time < 3600) // 18:00 - 19:00
            {
                Activate(Time + Exponential(6)); // HIGH RUSH
            }
            else if (current_time < 7200) // 19:00 - 20:00
            {
                Activate(Time + Exponential(10)); // MEDIUM RUSH
            }
            else if (current_time < 10800) // 20:00 - 21:00
            {
                Activate(Time + Exponential(20)); // LOW RUSH
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
    Print("Simulation done.");
    Print("Marta 1");
    marta1.Output();
    Print("Marta 2");
    marta2.Output();
    Print("kotva");
    kotva.Output();

    return 0;
}