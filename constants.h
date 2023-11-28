//
// Created by daniel on 28.11.23.
//

#ifndef IMS_PROJECT_CONSTANTS_H
#define IMS_PROJECT_CONSTANTS_H
#include "simlib.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <cstring>

/// time to lift the skier uphill
extern const int way_up_marta1 = 300;
extern const int way_up_marta2 = 180;
extern const int way_up_kotva = 300;
extern const int way_up_poma = 180;

//frequency of lift
extern const int departure_kotva = 12;
extern const int departure_poma = 12;
extern const int departure_marta1 = 12;
extern const int departure_marta2 = 6;

//ride down
extern const int poma_marta1 = 84;
extern const int poma_marta2 = 78;
extern const int marta2_marta1 = 168;
extern const int marta1_marta1 = 108;
extern const int marta2_marta2 = 108;
extern const int slunecna = 36;


extern const int pause_duration  = 300; // kolik si da cca pauzu, 5 min

extern const double day_time = 27000;

bool open = false;

enum lift
{
    MARTA1, // 0
    MARTA2, // 1
    KOTVA,   // 2
    POMA//3
};

int skier_cnt = 0;

Store marta1(2);
Store marta2(4);
Store kotva(2);
Facility poma;

extern std::vector<std::pair<double, double>> skier_stats;



#endif //IMS_PROJECT_CONSTANTS_H
