/**
 * VUT FIT IMS project - Freshbox food distribution.
 *
 * @file Work shift process implementation.
 * @author Dominik Harmim <xharmi00@stud.fit.vutbr.cz>
 * @author Vojtech Hertl <xhertl04@stud.fit.vutbr.cz>
 */

#include "constants.h"
/**
 * @struct Contains information about each Skier that leaves the system 
 */
std::vector<std::pair<double, double>> skier_stats;

/**
 * @class Represents working hours of ski centre
 * Depends on open_time
 */
class Open_hours : public Process
{
private:
    double open_time;

public:
    // Constructor
    explicit Open_hours(double openTime) : open_time(openTime) {}

    void Behavior() override
    {
        open = true;
        Wait(open_time); // Wait for the specified duration
        open = false;
        Cancel(); // Cancel the process
    }
};

/**
 * @class Defines one visitor of the ski centre
 * Defines his behavior and attributes necessary for the simulation
 */
class Skier : public Process
{
    int rides = 10;               /// number of rides until the skier leaves system
    lift current_lift;            /// which next lift will take the skier
    lift first_lift;              /// initial lift when entering the system
    double time_spent_skiing = 0; /// time spend on the slopes
    double time_spent_other = 0;  /// time spent waiting, transporting or having pause
    double start_time;            /// start at which the skier entered the system
    double pause;                 /// how long will the skier take pause
    int pause_cnt;                /// how many pauses have already taken

    /**
     * @brief Method defines the first lift that will the skier take
     */
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
        }
        else if (val < 0.75)
        {

            current_lift = POMA;
        }
        else
        {
            current_lift = KOTVA;
        }

        first_lift = current_lift;
    }
    /**
     * @brief Checks for time to take pause
     */
    void check_pause()
    {

        if (Time > this->start_time + pause * pause_cnt)
        {
            Wait(Exponential(pause_duration));
            pause_cnt++;
        }

    }

    void choose_bottom_marta2(){
        if(poma_improved){
            if (marta2.QueueLen() <= poma_improved_to_kotva.QueueLen())
            { /// choosing lift at the bottom of marta 2 and poma
                current_lift = MARTA2;
            }
            else
            {
                current_lift = POMA;
            }
        }
        else{
            if (marta2.QueueLen() <= poma.QueueLen())
            { /// choosing lift at the bottom of marta 2 and poma
                current_lift = MARTA2;
            }
            else
            {
                current_lift = POMA;
            }
        }

    }

    void choose_bottom_marta1(){
        if(marta_1_imporved){
            if (marta1_improved.QueueLen() <= kotva.QueueLen() )
            { /// choosing lift at the bottom of martha 1 and kotva
                current_lift = MARTA1;
            }
            else
            {
                current_lift = KOTVA;
            }

        }else{
            if (marta1.QueueLen() <= kotva.QueueLen() )
            { /// choosing lift at the bottom of martha 1 and kotva
                current_lift = MARTA1;
            }
            else
            {
                current_lift = KOTVA;
            }
        }

    }


    /**
     * @brief Defines behaviour of the skier when riding down the slopes
     */
    void ride_down()
    {
        if (current_lift == MARTA1 || current_lift == KOTVA) /// top of marta 1
        {
            double val = Random();

            if (val < 0.5) /// ride down slope marta 1
            {
                Wait(std::max(Normal(marta1_marta1, 10), 0.0));
                choose_bottom_marta1();
            }
            else /// ride down slunecna slope
            {
                Wait(std::max(Normal(slunecna, 10), 0.0));
                val = Random();
                if (val < 0.9) /// continues to marta 2 slope
                {
                    Wait(Normal(marta2_marta2, 10));
                    choose_bottom_marta2();

                }
                else /// continues back to the bottom of martha 1
                {
                    Wait(std::max(Normal(marta2_marta1, 10), 0.0));
                    choose_bottom_marta1();
                }
            }
        }
        else if(current_lift == POMA){ /// skier just took the lift poma

            double val = Random();
            if (val < 0.7) /// takes the slope back to bottom of martha 2 and poma
            {
                Wait(std::max(Normal(poma_marta2, 10), 0.0));
                choose_bottom_marta2();
            }
            else /// switches the slope and continues to martha 1 and kotva
            {
                Wait(std::max(Normal(poma_marta1, 10), 0.0));
                choose_bottom_marta1();
            }
        }
        else  ///skier just took the lift martha 2
        {
            double val = Random();
            if (val < 0.7)  /// continues back on martha 2 slope
            {
                Wait(std::max(Normal(marta2_marta2, 10), 0.0));
                choose_bottom_marta2();
            }
            else  ///switches and continues to martha 1 and kotva
            {
                Wait(std::max(Normal(marta2_marta1, 10), 0.0));
               choose_bottom_marta1();
            }
        }
        check_pause();
    }

    /**
     * @brief Defines the process of taking the skier up
     */
    void ride_up()
    {

        if (current_lift == MARTA1)
        {
            if(marta_1_imporved){
                Enter(marta1_improved, 1);
                Wait(departure_marta2);
                Leave(marta1_improved, 1);
                Wait(way_up_marta1_improved);
            }else{
                Enter(marta1, 1); /// seize place at the platform
                Wait(departure_marta1); ///wait for lift
                Leave(marta1, 1); /// free the place (sit on the lift and go)
                Wait(way_up_marta1); /// going with the lift up
            }

        }
        else if (current_lift == MARTA2)
        {
            Enter(marta2, 1);
            Wait(departure_marta2);
            Leave(marta2, 1);
            Wait(way_up_marta2);
        }
        else if (current_lift == POMA){
            if(poma_improved){
                Enter(poma_improved_to_kotva, 1);
                Wait(departure_kotva);
                Leave(poma_improved_to_kotva, 1);
                Wait(way_up_poma_improved);
            }else{
                Seize(poma);
                Wait(departure_poma);
                Release(poma);
                Wait(way_up_poma);
            }

        }
        else{ // KOTVA
            Enter(kotva, 1);
            Wait(departure_kotva);
            Leave(kotva, 1);
            Wait(way_up_kotva);
        }
    }
    /**
     * @brief Defines behavior of process Skier
     */
    void Behavior() override
    {
        start_time = Time;
        pause = Exponential(day_time/4); /// will take approx. 4 pauses per day time
        pause_cnt = 1;
        choose_start(); /// spawn at the bottom of some lift
        while (rides > 0 && open) /// while have rides and the ski areal is open
        {
            double tmp = Time;
            ride_up();
            time_spent_other += (Time - tmp); /// compute other activities
            tmp = Time;
            ride_down();
            time_spent_skiing += (Time - tmp); /// compute skiing
            rides--;
        }
        ///compute the ration of skiing to other activities
        double skiing_ratio;
        if(rides == 0){
             skiing_ratio = time_spent_skiing/(time_spent_skiing+time_spent_other)*100;
        }else{
             skiing_ratio =0;
        }

        skier_stats.emplace_back(start_time, skiing_ratio); ///save the stats

        Terminate(); ///skier leaves the system
    }
};

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

    marta_1_imporved = false;
    poma_improved = false;
    kotva_closing = false;
    poma_closing = false;

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

    /// print statistics about the skiers
    std::sort(skier_stats.begin(), skier_stats.end(),
              [](const std::pair<double, double> &lhs, const std::pair<double, double> &rhs)
              {
                  return lhs.first < rhs.first;
              });
    // Access and print the values in the vector
    for (const auto &pair : skier_stats)
    {
        Print("Start: ");
        Print(pair.first);
        Print(" Ratio: ");
        Print(pair.second);
        Print("%\n");
    }
    /// output for the user

    Print("Simulation done.\n");
    Print("Total number of visitors.");
    Print(skier_cnt);
    Print("\n");
    if(marta_1_imporved){
        Print("Marta 1 improved");
        marta1_improved.Output();
    }else{
        Print("Marta 1 improved");
        marta1.Output();
    }

    Print("Marta 2");
    marta2.Output();
    Print("kotva");
    kotva.Output();
    if(poma_improved){
        Print("poma improved");
        poma_improved_to_kotva.Output();
    }else{
        Print("poma");
        poma.Output();
    }



    return 0;
}