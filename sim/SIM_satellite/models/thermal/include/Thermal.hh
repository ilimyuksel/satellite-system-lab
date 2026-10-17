/************************ TRICK HEADER *************************
PURPOSE: (Three-node lumped thermal network with powered battery thermostat.)
LIBRARY DEPENDENCIES: ((thermal/src/Thermal.cpp))
***************************************************************/
#ifndef LAB_THERMAL_HH
#define LAB_THERMAL_HH
class Thermal {
public:
    double mass[3]; /* kg Body, battery, payload thermal masses */
    double cp[3]; /* J/kg/K Specific heat capacities */
    double initial_k[3]; /* K Initial temperatures */
    double temperature_k[3]; /* K State: body, battery, payload */
    double temperature_c[3]; /* -- Temperatures in Celsius for telemetry */
    double conductance_battery; /* W/K Body-battery conductive coupling */
    double conductance_payload; /* W/K Body-payload conductive coupling */
    double solar_area; /* m2 Prescribed projected non-PV body area */
    double absorptivity; /* -- Solar absorptivity of body */
    double radiating_area; /* m2 Body radiating area */
    double emissivity; /* -- Body IR emissivity */
    double earth_ir_w_m2; /* W/m2 Prescribed Earth IR irradiance */
    double earth_view_factor; /* -- Prescribed area-averaged Earth view factor */
    double space_k; /* K Background radiation temperature */
    bool heater_enabled; /* -- Enable thermostat */
    double heater_on_k; /* K Request heat below this temperature */
    double heater_off_k; /* K Stop heat above this temperature */
    double heater_rating_w; /* W Electrical heater demand */
    bool heater_requested; /* -- Thermostat command, distinct from delivered heat */
    double heater_served_w; /* W Actually powered heater; all dissipated in battery */
    double solar_absorbed_w; /* W Absorbed body solar power; excludes PV panels */
    double earth_absorbed_w; /* W Absorbed prescribed Earth IR */
    double internal_w[3]; /* W Held internal sources by node, heater included */
    double radiated_w; /* W Net outgoing radiation to background */
    double body_to_battery_w; /* W Positive body to battery conduction */
    double body_to_payload_w; /* W Positive body to payload conduction */
    double net_energy_j; /* J Integrated net heat across network boundary */
    double balance_error_j; /* J Change in stored thermal energy minus net input */
    double heater_energy_wh; /* -- Delivered heater energy in Wh */
    int default_data();
    int initialize();
    int advance(double dt);
    int thermostat();
    int sources(double visibility,double distance,double body_w,double battery_w,double payload_w,double heater_w);
    int diagnostics();
};
#endif
