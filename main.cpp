/**
 * VUT FIT IMS project - ski centre simulation
 *
 * @file main.cpp
 * @author Matěj Macek <xmacek27@stud.fit.vutbr.cz>
 * @author Daniel Žárský <xzarsk04@stud.fit.vutbr.cz>
 */

#include "constants.h"

bool marta_1_improved = false; // true = marta 1 improved
bool poma_improved = false;    // true = poma improved
bool poma_optimized = false;   // true = poma optimized
double speed_up = 0;           // speed up the generation of skiers
double day_time = 0;           // 8:30 - 15:00
double night_time = 0;         // 18:00 - 21:00
double start_using_poma = 0;   // time when first skier used poma in poma_optimized
double end_using_poma = 0;     // time when last skier used poma in poma_optimized
bool first = true;             // true = first skier used poma in poma_optimized
bool open = false;             // true = ski centre is open
int unfinished_rides = 0;      // number of unfinished rides

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
    int rides = 9;                /// number of rides until the skier leaves system
    lift current_lift;            /// which next lift will take the skier
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

    void choose_bottom_marta2()
    {
        if (poma_improved)
        {
            if (marta2.QueueLen() / marta2.Capacity() <= poma_improved_to_kotva.QueueLen() / poma_improved_to_kotva.Capacity())
            { /// choosing lift at the bottom of marta 2 and poma
                current_lift = MARTA2;
            }
            else
            {
                current_lift = POMA;
            }
        }
        else
        {
            if (poma_optimized)
            {
                if (marta2.QueueLen() / marta2.Capacity() <= poma.QueueLen() / 1 + 32)
                { /// choosing lift at the bottom of marta 2 and poma
                    current_lift = MARTA2;
                }
                else
                {
                    if (first)
                    {
                        start_using_poma = Time;
                        first = false;
                    }
                    current_lift = POMA;
                    end_using_poma = Time;
                }
            }
            else
            {
                if (marta2.QueueLen() / marta2.Capacity() <= poma.QueueLen() / 1)
                { /// choosing lift at the bottom of marta 2 and poma
                    current_lift = MARTA2;
                }
                else
                {
                    current_lift = POMA;
                }
            }
        }
    }

    void choose_bottom_marta1()
    {
        if (marta_1_improved)
        {
            if (marta1_improved.QueueLen() / marta1_improved.Capacity() <= kotva.QueueLen() / kotva.Capacity())
            { /// choosing lift at the bottom of martha 1 and kotva
                current_lift = MARTA1;
            }
            else
            {
                current_lift = KOTVA;
            }
        }
        else
        {
            if (marta1.QueueLen() / marta1.Capacity() <= kotva.QueueLen() / kotva.Capacity())
            { /// choosing lift at the bottom of martha 1 and kotva
                current_lift = MARTA1;
            }
            else
            {
                current_lift = KOTVA;
            }
        }
    }

    /// true = ride down to marta 1
    bool choose_ride_down()
    {
        if (marta_1_improved && !poma_improved)
        {
            if ((marta1_improved.QueueLen() + kotva.QueueLen()) / (marta1_improved.Capacity() * 2 + kotva.Capacity()) < (marta2.QueueLen() + poma.QueueLen()) / (marta2.Capacity() * 2 + 1))
            {
                return true;
            }
        }
        else if (poma_improved && !marta_1_improved)
        {
            if ((marta1.QueueLen() + kotva.QueueLen()) / (marta1.Capacity() + kotva.Capacity()) < (marta2.QueueLen() + poma_improved_to_kotva.QueueLen()) / (marta2.Capacity() + poma_improved_to_kotva.Capacity()))
            {
                return true;
            }
        }
        else if (poma_improved && marta_1_improved)
        {
            if ((marta1_improved.QueueLen() + kotva.QueueLen()) / (marta1_improved.Capacity() + kotva.Capacity()) < (marta2.QueueLen() + poma_improved_to_kotva.QueueLen()) / (marta2.Capacity() + poma_improved_to_kotva.Capacity()))
            {
                return true;
            }
        }
        else
        {
            if ((marta1.QueueLen() + kotva.QueueLen()) / (marta1.Capacity() + kotva.Capacity()) < ((marta2.QueueLen() + poma.QueueLen()) / (marta2.Capacity() * 2 + 1)))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Defines behaviour of the skier when riding down the slopes
     */
    void ride_down()
    {
        if (current_lift == MARTA1 || current_lift == KOTVA) /// top of marta 1
        {

            if (choose_ride_down()) /// ride down slope marta 1
            {
                Wait(std::max(Normal(marta1_marta1, 10), 0.0));
                choose_bottom_marta1();
            }
            else /// ride down slunecna slope
            {
                Wait(std::max(Normal(slunecna, 10), 0.0));

                if (choose_ride_down())
                {

                    Wait(std::max(Normal(marta2_marta1, 10), 0.0));
                    choose_bottom_marta1(); /// continues back to the bottom of martha 1
                }
                else
                {
                    Wait(Normal(marta2_marta2, 10));
                    choose_bottom_marta2(); /// continues to marta 2 slope
                }
            }
        }
        else if (current_lift == POMA)
        { /// skier just took the lift poma

            if (choose_ride_down())
            {
                Wait(std::max(Normal(poma_marta1, 10), 0.0));
                choose_bottom_marta1(); /// switches the slope and continues to martha 1 and kotva
            }
            else
            {
                Wait(std::max(Normal(poma_marta2, 10), 0.0));
                choose_bottom_marta2(); /// takes the slope back to bottom of martha 2 and poma
            }
        }
        else /// skier just took the lift martha 2
        {

            if (choose_ride_down())
            {
                Wait(std::max(Normal(marta2_marta1, 10), 0.0));
                choose_bottom_marta1(); /// switches and continues to martha 1 and kotva
            }
            else
            {

                Wait(std::max(Normal(marta2_marta2, 10), 0.0));
                choose_bottom_marta2(); /// continues back on martha 2 slope
            }
        }
    }

    /**
     * @brief Defines the process of taking the skier up
     */
    void ride_up()
    {

        if (current_lift == MARTA1)
        {
            if (marta_1_improved)
            {
                Enter(marta1_improved, 1);
                Wait(departure_marta2);
                Leave(marta1_improved, 1);
                Wait(way_up_marta1_improved);
            }
            else
            {
                Enter(marta1, 1);       /// seize place at the platform
                Wait(departure_marta1); /// wait for lift
                Leave(marta1, 1);       /// free the place (sit on the lift and go)
                Wait(way_up_marta1);    /// going with the lift up
            }
        }
        else if (current_lift == MARTA2)
        {
            Enter(marta2, 1);
            Wait(departure_marta2);
            Leave(marta2, 1);
            Wait(way_up_marta2);
        }
        else if (current_lift == POMA)
        {

            if (poma_improved)
            {
                Enter(poma_improved_to_kotva, 1);
                Wait(departure_kotva);
                Leave(poma_improved_to_kotva, 1);
                Wait(way_up_poma_improved);
            }
            else
            {
                Seize(poma);
                Wait(departure_poma);
                Release(poma);
                Wait(way_up_poma);
            }
        }
        else
        { // KOTVA
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
        pause = Exponential(day_time / 4); /// will take approx. 4 pauses per day time
        pause_cnt = 1;
        choose_start();           /// spawn at the bottom of some lift
        while (rides > 0 && open) /// while have rides and the ski areal is open
        {
            double tmp = Time;
            ride_up();
            time_spent_other += (Time - tmp); /// compute other activities
            tmp = Time;
            ride_down();
            check_pause();
            time_spent_skiing += (Time - tmp); /// compute skiing
            rides--;
        }
        /// compute the ration of skiing to other activities
        double skiing_ratio;
        if (rides == 0)
        {
            skiing_ratio = time_spent_skiing / (time_spent_skiing + time_spent_other) * 100;
        }
        else
        {
            skiing_ratio = time_spent_skiing / (time_spent_skiing + time_spent_other) * 100;
            unfinished_rides += rides;
        }

        skier_stats.emplace_back(start_time, skiing_ratio); /// save the stats

        Terminate(); /// skier leaves the system
    }
};

/**
 * @brief Class representing a generator for simulating events on a day.
 * Inherits from Event class.
 */
class GeneratorDay : public Event
{

    double current_time = 0.0; /**< Current simulation time. */

    /**
     * @brief Behavior function override from Event class.
     */
    void Behavior() override
    {
        current_time = Time;

        if (!open)
        {
            Cancel();
        }
        else
        {
            (new Skier)->Activate();
            skier_cnt++;
            double seed = -speed_up;

            if (current_time < 10800)
            {                                           // 8:30 - 11:00 (ULTRA RUSH)
                Activate(Time + Exponential(8 + seed)); // Adjusted rate for ULTRA RUSH hours
            }
            else if (current_time < 13200)
            {                                            // 11:00 - 12:00 (MEDIUM RUSH)
                Activate(Time + Exponential(12 + seed)); // Adjusted rate for MEDIUM RUSH hours
            }
            else if (current_time < 18000)
            {                                            // 12:00 - 13:00 (LOW RUSH)
                Activate(Time + Exponential(20 + seed)); // Adjusted rate for LOW RUSH hours
            }
            else if (current_time < 21600)
            {                                            // 13:00 - 15:00 (HIGH RUSH)
                Activate(Time + Exponential(12 + seed)); // Adjusted rate for HIGH RUSH hours
            }
            else if (current_time < 27000)
            {                                            // 15:00 - 16:00 (LOW RUSH)
                Activate(Time + Exponential(25 + seed)); // Adjusted rate for LOW RUSH hours
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
        }
        else
        {
            (new Skier)->Activate();
            skier_cnt++;

            double seed = -speed_up;
            // 18:00 - 21:00
            if (current_time < 3600) // 18:00 - 19:00
            {
                Activate(Time + Exponential(8 + seed)); // HIGH RUSH
            }
            else if (current_time < 7200) // 19:00 - 20:00
            {
                Activate(Time + Exponential(12 + seed)); // MEDIUM RUSH
            }
            else if (current_time < 10800) // 20:00 - 21:00
            {
                Activate(Time + Exponential(25 + seed)); // LOW RUSH
            }
        }
    }
};

/**
 * @brief Prints help message
 */
void print_help()
{
    printf("Usage: ./ims [day|night] [-m marta1Imp] [-p pomaImp|pomaOpti] [-s <speed_value>] [-h]\n");
    printf("Options:\n");
    printf("  -m marta1Imp       : Specify marta1Imp mode\n");
    printf("  -p pomaImp         : Specify pomaImp mode\n");
    printf("  -p pomaOpti        : Specify pomaOpti mode\n");
    printf("  -s <speed_value>   : Specify speed value (double)\n");
    printf("  -h                 : Display help message\n");
}

/**
 * @brief Parses command line arguments and set global variables accordingly
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @return True if day, false if night
 */
bool parse_args(int argc, char *argv[])
{
    bool returnValue = true; // Default return value

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            // Display help message and exit
            print_help();
            exit(0);
        }
        else if (strcmp(argv[i], "day") == 0)
        {
            returnValue = true;
        }
        else if (strcmp(argv[i], "night") == 0)
        {
            returnValue = false;
        }
        else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc)
        {
            // Handling for '-m' argument
            if (strcmp(argv[i + 1], "marta1Imp") == 0)
            {
                marta_1_improved = true;
            }
            else
            {
                print_help();
                exit(0);
            }
            ++i; // Skip the next argument as it's processed here
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
        {
            // Handling for '-p' argument
            if (strcmp(argv[i + 1], "pomaImp") == 0)
            {
                poma_improved = true;
                // Additional logic specific to pomaImp mode
            }
            else if (strcmp(argv[i + 1], "pomaOpti") == 0)
            {
                poma_optimized = true;
                // Additional logic specific to pomaOpti mode
            }
            else
            {
                print_help();
                exit(0);
            }
            ++i; // Skip the next argument as it's processed here
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc)
        {
            // Handling for '-s' argument for speed value
            speed_up = atof(argv[i + 1]);
            ++i; // Skip the next argument as it's processed here
        }
        else
        {
            print_help();
            exit(0);
        }
    }

    return returnValue; // Return the value after processing all arguments
}

int main(int argc, char *argv[])
{
    bool day = parse_args(argc, argv); // by default day if no args

    Print("\nProject IMS 2023\n");
    Init(0, 40000);
    day_time = 27000;
    night_time = 10800;

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

    Run();

    // print statistics about the skiers
    std::sort(skier_stats.begin(), skier_stats.end(),
              [](const std::pair<double, double> &lhs, const std::pair<double, double> &rhs)
              {
                  return lhs.first < rhs.first;
              });

    double sum = 0.0;
    for (const auto &pair : skier_stats)
    {
        sum += pair.second;
    }
    double average = skier_stats.empty() ? 0.0 : sum / skier_stats.size();

    Print("\nTotal number of visitors.");
    Print(skier_cnt);
    Print("\n");
    Print("Quality of skiing:");
    Print(average);
    Print("%\n");
    Print("Unfinished rides:");
    Print(unfinished_rides);
    Print("\n");
    if (marta_1_improved)
    {
        Print("Marta 1 improved \n");
        marta1_improved.Output();
    }
    else
    {
        Print("Marta 1 \n");
        marta1.Output();
    }
    Print("Marta 2 \n");
    marta2.Output();
    Print("Kotva \n");
    kotva.Output();
    if (poma_improved)
    {
        Print("Poma improved\n");
        poma_improved_to_kotva.Output();
    }
    else
    {
        Print("Poma \n");
        poma.Output();
    }
    if (poma_optimized)
    {
        Print("Start using poma:");
        Print(start_using_poma);
        Print("\n");
        Print("End using poma:");
        Print(end_using_poma);
        Print("\n");
        double res = end_using_poma - start_using_poma;
        Print("Poma ration:");
        Print(((res + way_up_poma + departure_poma) / day_time) * 100);
        Print("%\n");
    }

    return 0;
}