/**
 * VUT FIT IMS project - ski centre simulation
 *
 * @file constants.h
 * @author Matěj Macek <xmacek27@stud.fit.vutbr.cz>
 * @author Daniel Žárský <xzarsk04@stud.fit.vutbr.cz>
 */

#ifndef IMS_PROJECT_CONSTANTS_H
#define IMS_PROJECT_CONSTANTS_H
#include "simlib.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <cstring>

enum lift
{
    MARTA1, // 0
    MARTA2, // 1
    KOTVA,  // 2
    POMA    // 3
};

/// time to lift the skier uphill
extern const int way_up_marta1 = 300;
extern const int way_up_marta1_improved = 300;
extern const int way_up_poma_improved = 210;
extern const int way_up_marta2 = 180;
extern const int way_up_kotva = 300;
extern const int way_up_poma = 210;

// frequency of lift
extern const int departure_kotva = 12;
extern const int departure_poma = 12;
extern const int departure_marta1 = 12;
extern const int departure_marta2 = 6;

// ride down
extern const int poma_marta1 = 84;
extern const int poma_marta2 = 78;
extern const int marta2_marta1 = 168;
extern const int marta1_marta1 = 108;
extern const int marta2_marta2 = 108;
extern const int slunecna = 36;

/// EXPERIMENTY
extern bool marta_1_improved;
extern bool poma_improved;

extern double speed_up;

extern double day_time;
extern double night_time;

extern bool poma_optimized;

extern bool first;
extern double start_using_poma;
extern double end_using_poma;

extern bool open;
extern int unfinished_rides;
extern const int pause_duration = 300; // kolik si da cca pauzu, 5 min

int skier_cnt = 0;

Store marta1(2);
Store marta2(4);
Store kotva(2);
Store marta1_improved(4);
Store poma_improved_to_kotva(2);
Facility poma;

extern std::vector<std::pair<double, double>> skier_stats;

#endif // IMS_PROJECT_CONSTANTS_H
