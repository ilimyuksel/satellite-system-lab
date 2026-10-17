#include "power/include/Power.hh"
#include <algorithm>
#include <cmath>
#include <stdexcept>
int Power::default_data(){
    capacity_wh=80;initial_soc=.65;panel_area=.12;panel_efficiency=.28;
    incidence_cosine=.7;regulation_efficiency=.9;charge_efficiency=.95;discharge_efficiency=.95;
    max_charge_w=30;max_discharge_w=40;base_load_w=8;cutoff_soc=.2;recovery_soc=.35;
    capacity_soh=1;ocv_empty_v=12;ocv_full_v=16.8;internal_resistance=.08;
    min_terminal_v=11.2;max_terminal_v=16.8;max_charge_a=2;max_discharge_a=3;
    panel_health=1;
    return 0;
}
int Power::initialize(){
    for(double v:{capacity_wh,initial_soc,panel_area,panel_efficiency,incidence_cosine,
      regulation_efficiency,charge_efficiency,discharge_efficiency,max_charge_w,max_discharge_w,
      base_load_w,cutoff_soc,recovery_soc,capacity_soh,ocv_empty_v,ocv_full_v,internal_resistance,min_terminal_v,max_terminal_v,max_charge_a,max_discharge_a,panel_health})if(!std::isfinite(v))throw std::runtime_error("Non-finite EPS parameter");
    if(!(capacity_wh>0&&initial_soc>=0&&initial_soc<=1&&panel_area>=0&&panel_efficiency>=0&&panel_efficiency<=1&&
       incidence_cosine>=0&&incidence_cosine<=1&&regulation_efficiency>0&&regulation_efficiency<=1&&
       charge_efficiency>0&&charge_efficiency<=1&&discharge_efficiency>0&&discharge_efficiency<=1&&
       max_charge_w>=0&&max_discharge_w>=0&&base_load_w>=0&&cutoff_soc>=0&&cutoff_soc<recovery_soc&&recovery_soc<=1))
       throw std::runtime_error("Invalid EPS capacity, efficiency, limit, or hysteresis threshold");
    if(!(capacity_soh>0&&capacity_soh<=1&&ocv_empty_v>0&&ocv_full_v>=ocv_empty_v&&
      internal_resistance>=0&&min_terminal_v>0&&min_terminal_v<ocv_full_v&&
      max_terminal_v>=ocv_full_v&&max_charge_a>=0&&max_discharge_a>=0&&panel_health>=0&&panel_health<=1))
      throw std::runtime_error("Invalid battery circuit or health parameter");
    effective_capacity_wh=capacity_wh*capacity_soh;
    energy_wh=effective_capacity_wh*initial_soc;soc=initial_soc;
    ocv_v=ocv_empty_v+(ocv_full_v-ocv_empty_v)*soc;terminal_v=ocv_v;
    battery_current_a=ohmic_heat_w=converter_loss_w=discharge_limit_w=charge_limit_w=0;
    solar_w=requested_w=served_w=unserved_w=charge_w=discharge_w=curtailed_w=loss_w=energy_rate_wh_s=0;
    harvested_wh=supplied_wh=curtailed_wh=losses_wh=balance_error_wh=bus_residual_w=0;
    return 0;
}
int Power::advance(double dt){
    if(dt<0||!std::isfinite(dt))throw std::runtime_error("Invalid EPS timestep");
    energy_wh+=energy_rate_wh_s*dt;
    if(energy_wh< -1e-9||energy_wh>effective_capacity_wh+1e-9)throw std::runtime_error("EPS storage bounds violated");
    energy_wh=std::clamp(energy_wh,0.0,effective_capacity_wh);soc=energy_wh/effective_capacity_wh;
    harvested_wh+=solar_w*dt/3600;supplied_wh+=served_w*dt/3600;
    curtailed_wh+=curtailed_w*dt/3600;losses_wh+=loss_w*dt/3600;
    balance_error_wh=energy_wh-effective_capacity_wh*initial_soc-(harvested_wh-supplied_wh-curtailed_wh-losses_wh);
    ocv_v=ocv_empty_v+(ocv_full_v-ocv_empty_v)*soc;
    return 0;
}
int Power::solar(double visibility,double distance){
    if(!std::isfinite(visibility)||visibility<0||visibility>1||!std::isfinite(distance)||distance<=0)
        throw std::runtime_error("Invalid solar environment");
    const double au=149597870700.0;
    solar_w=1361*(au/distance)*(au/distance)*panel_area*panel_efficiency*incidence_cosine*regulation_efficiency*visibility*panel_health;
    return 0;
}
double Power::available(double dt){
    if(!(dt>0))throw std::runtime_error("Positive EPS hold interval required");
    double limit=std::min(max_discharge_a,energy_wh*3600/(ocv_v*dt));
    if(ocv_v<=min_terminal_v)limit=0;
    else if(internal_resistance>0)limit=std::min({limit,(ocv_v-min_terminal_v)/internal_resistance,ocv_v/(2*internal_resistance)});
    discharge_limit_w=std::min(max_discharge_w,discharge_efficiency*(ocv_v*limit-internal_resistance*limit*limit));
    return solar_w+discharge_limit_w;
}
int Power::allocate(double requested,double actual_demand,double dt){
    if(!std::isfinite(requested)||!std::isfinite(actual_demand)||requested<0||actual_demand<0||actual_demand>requested+1e-9)throw std::runtime_error("Invalid power demand");
    requested_w=requested;served_w=std::min(actual_demand,available(dt));unserved_w=requested-served_w;
    charge_w=discharge_w=curtailed_w=battery_current_a=0;
    double ic=std::min(max_charge_a,(effective_capacity_wh-energy_wh)*3600/(ocv_v*dt));
    if(internal_resistance>0)ic=std::min(ic,std::max(0.0,(max_terminal_v-ocv_v)/internal_resistance));
    charge_limit_w=std::min(max_charge_w,(ocv_v*ic+internal_resistance*ic*ic)/charge_efficiency);
    if(solar_w>=served_w){
        charge_w=std::min(solar_w-served_w,charge_limit_w);
        curtailed_w=solar_w-served_w-charge_w;
        double power=charge_w*charge_efficiency;
        battery_current_a=-2*power/(ocv_v+std::sqrt(ocv_v*ocv_v+4*internal_resistance*power));
    }else{
        discharge_w=served_w-solar_w;
        double power=discharge_w/discharge_efficiency;
        battery_current_a=2*power/(ocv_v+std::sqrt(std::max(0.0,ocv_v*ocv_v-4*internal_resistance*power)));
    }
    terminal_v=ocv_v-internal_resistance*battery_current_a;
    ohmic_heat_w=internal_resistance*battery_current_a*battery_current_a;
    converter_loss_w=charge_w*(1-charge_efficiency)+discharge_w*(1/discharge_efficiency-1);
    loss_w=converter_loss_w+ohmic_heat_w;
    energy_rate_wh_s=-ocv_v*battery_current_a/3600;
    bus_residual_w=solar_w+discharge_w-served_w-charge_w-curtailed_w;
    return 0;
}
