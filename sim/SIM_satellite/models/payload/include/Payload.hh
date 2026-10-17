/************************ TRICK HEADER *************************
PURPOSE: (Scheduled payload load; no instrument data or thermal state.)
LIBRARY DEPENDENCIES: ((payload/src/Payload.cpp))
***************************************************************/
#ifndef LAB_PAYLOAD_HH
#define LAB_PAYLOAD_HH
class Payload {
public:
    double active_w; /* W Active power demand */
    double standby_w; /* W Standby power demand */
    double first_on_s; /* s First activity request */
    double cycle_s; /* s Schedule repetition interval */
    double on_duration_s; /* s Requested active duration per cycle */
    bool commanded; /* -- Requested active state */
    int mode; /* -- 0 off, 1 standby, 2 active */
    double requested_w; /* W Commanded power */
    double consumed_w; /* W Supplied payload power */
    double active_seconds; /* s Accumulated actual active duration */
    double data_rate_mbps; /* -- Produced megabits per second in active mode */
    double heat_fraction; /* -- Fraction of consumed electrical power dissipated locally */
    double generated_data_mbit; /* -- Cumulative produced data; no storage/downlink model */
    double heat_w; /* W Payload thermal source, not temperature */
    int default_data();
    int initialize();
    int command(double time);
};
#endif
