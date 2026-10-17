/************************ TRICK HEADER *************************
PURPOSE: (Lumped energy storage and power bus, not battery electrochemistry.)
LIBRARY DEPENDENCIES: ((power/src/Power.cpp))
***************************************************************/
#ifndef LAB_POWER_HH
#define LAB_POWER_HH
class Power {
public:
    double capacity_wh; /* -- Battery usable energy capacity in Wh */
    double initial_soc; /* -- Initial fractional state of charge */
    double panel_area; /* m2 Effective photovoltaic area */
    double panel_efficiency; /* -- Constant photovoltaic conversion efficiency */
    double incidence_cosine; /* -- Prescribed panel incidence, no ADCS yet */
    double regulation_efficiency; /* -- Solar regulator efficiency */
    double charge_efficiency; /* -- Battery charge efficiency */
    double discharge_efficiency; /* -- Battery discharge efficiency */
    double max_charge_w; /* W Battery bus charge limit */
    double max_discharge_w; /* W Battery bus discharge limit */
    double base_load_w; /* W Always requested platform power */
    double cutoff_soc; /* -- Payload shedding threshold */
    double recovery_soc; /* -- Payload restart threshold */
    double energy_wh; /* -- Stored energy in Wh */
    double soc; /* -- Fractional state of charge */
    double solar_w; /* W Regulated solar power */
    double requested_w; /* W Platform plus commanded payload demand */
    double served_w; /* W Actually supplied power */
    double unserved_w; /* W Requested minus served power */
    double charge_w; /* W Bus power directed into battery */
    double discharge_w; /* W Bus power supplied by battery */
    double curtailed_w; /* W Unused solar power */
    double loss_w; /* W Battery conversion losses */
    double energy_rate_wh_s; /* -- Stored energy rate, Wh/s */
    double harvested_wh; /* -- Integrated regulated solar energy, Wh */
    double supplied_wh; /* -- Integrated load energy, Wh */
    double curtailed_wh; /* -- Integrated curtailed energy, Wh */
    double losses_wh; /* -- Integrated battery conversion losses, Wh */
    double balance_error_wh; /* -- Storage minus accounted net energy, Wh */
    double bus_residual_w; /* W Instantaneous bus power residual */
    double capacity_soh; /* -- Fixed capacity health factor, not dynamic ageing */
    double effective_capacity_wh; /* -- Usable capacity after health scaling, Wh */
    double ocv_empty_v; /* V Synthetic open circuit voltage at zero energy fraction */
    double ocv_full_v; /* V Synthetic open circuit voltage at full energy fraction */
    double internal_resistance; /* ohm Pack series resistance */
    double min_terminal_v; /* V Minimum discharge terminal voltage */
    double max_terminal_v; /* V Maximum charge terminal voltage */
    double max_charge_a; /* A Charge current limit */
    double max_discharge_a; /* A Discharge current limit */
    double panel_health; /* -- Fixed panel performance factor */
    double ocv_v; /* V Open circuit voltage */
    double terminal_v; /* V Loaded battery terminal voltage */
    double battery_current_a; /* A Positive discharge, negative charge */
    double ohmic_heat_w; /* W Battery resistive heat, no temperature model */
    double converter_loss_w; /* W Battery converter losses */
    double discharge_limit_w; /* W Available battery bus power */
    double charge_limit_w; /* W Available battery charging bus power */
    int default_data();
    int initialize();
    int advance(double dt);
    double available(double dt);
    int solar(double visibility, double distance);
    int allocate(double requested, double actual_demand, double dt);
};
#endif
