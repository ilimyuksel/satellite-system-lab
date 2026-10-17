/************************ TRICK HEADER *************************
PURPOSE: (One-second coupling of environment, EPS and scheduled payload.)
LIBRARY DEPENDENCIES: ((system/src/Spacecraft.cpp))
***************************************************************/
#ifndef LAB_SPACECRAFT_HH
#define LAB_SPACECRAFT_HH
#include "power/include/Power.hh"
#include "payload/include/Payload.hh"
class Spacecraft {
public:
    Power eps;
    Payload payload;
    bool low_power; /* -- Hysteretic payload inhibition */
    bool power_limited; /* -- Payload requested active but denied */
    double last_time; /* s Last subsystem update */
    int default_data();
    int initialize(double visibility, double sun_distance);
    int step(double time, double visibility, double sun_distance);
};
#endif
