#include "power/include/Power.hh"
#include <algorithm>
#include <cmath>
#include <stdexcept>
int Power::default_data(){
    capacity_wh=80;initial_soc=.65;panel_area=.12;panel_efficiency=.28;
    incidence_cosine=.7;regulation_efficiency=.9;charge_efficiency=.95;discharge_efficiency=.95;
    max_charge_w=30;max_discharge_w=40;base_load_w=8;cutoff_soc=.2;recovery_soc=.35;
    return 0;
}
int Power::initialize(){
    for(double v:{capacity_wh,initial_soc,panel_area,panel_efficiency,incidence_cosine,
      regulation_efficiency,charge_efficiency,discharge_efficiency,max_charge_w,max_discharge_w,
      base_load_w,cutoff_soc,recovery_soc})if(!std::isfinite(v))throw std::runtime_error("Non-finite EPS parameter");
    if(!(capacity_wh>0&&initial_soc>=0&&initial_soc<=1&&panel_area>=0&&panel_efficiency>=0&&panel_efficiency<=1&&
       incidence_cosine>=0&&incidence_cosine<=1&&regulation_efficiency>0&&regulation_efficiency<=1&&
       charge_efficiency>0&&charge_efficiency<=1&&discharge_efficiency>0&&discharge_efficiency<=1&&
       max_charge_w>=0&&max_discharge_w>=0&&base_load_w>=0&&cutoff_soc>=0&&cutoff_soc<recovery_soc&&recovery_soc<=1))
       throw std::runtime_error("Invalid EPS capacity, efficiency, limit, or hysteresis threshold");
    energy_wh=capacity_wh*initial_soc;soc=initial_soc;
    solar_w=requested_w=served_w=unserved_w=charge_w=discharge_w=curtailed_w=loss_w=energy_rate_wh_s=0;
    harvested_wh=supplied_wh=curtailed_wh=losses_wh=balance_error_wh=bus_residual_w=0;
    return 0;
}
int Power::advance(double dt){
    if(dt<0||!std::isfinite(dt))throw std::runtime_error("Invalid EPS timestep");
    energy_wh+=energy_rate_wh_s*dt;
    if(energy_wh< -1e-9||energy_wh>capacity_wh+1e-9)throw std::runtime_error("EPS storage bounds violated");
    energy_wh=std::clamp(energy_wh,0.0,capacity_wh);soc=energy_wh/capacity_wh;
    harvested_wh+=solar_w*dt/3600;supplied_wh+=served_w*dt/3600;
    curtailed_wh+=curtailed_w*dt/3600;losses_wh+=loss_w*dt/3600;
    balance_error_wh=energy_wh-capacity_wh*initial_soc-(harvested_wh-supplied_wh-curtailed_wh-losses_wh);
    return 0;
}
int Power::solar(double visibility,double distance){
    if(!std::isfinite(visibility)||visibility<0||visibility>1||!std::isfinite(distance)||distance<=0)
        throw std::runtime_error("Invalid solar environment");
    const double au=149597870700.0;
    solar_w=1361*(au/distance)*(au/distance)*panel_area*panel_efficiency*incidence_cosine*regulation_efficiency*visibility;
    return 0;
}
double Power::available(double dt){
    if(!(dt>0))throw std::runtime_error("Positive EPS hold interval required");
    return solar_w+std::min(max_discharge_w,energy_wh*3600*discharge_efficiency/dt);
}
int Power::allocate(double requested,double actual_demand,double dt){
    if(requested<0||actual_demand<0||actual_demand>requested+1e-9)throw std::runtime_error("Invalid power demand");
    requested_w=requested;served_w=std::min(actual_demand,available(dt));unserved_w=requested-served_w;
    charge_w=discharge_w=curtailed_w=0;
    if(solar_w>=served_w){
        charge_w=std::min({solar_w-served_w,max_charge_w,(capacity_wh-energy_wh)*3600/(charge_efficiency*dt)});
        curtailed_w=solar_w-served_w-charge_w;
    }else discharge_w=served_w-solar_w;
    loss_w=charge_w*(1-charge_efficiency)+discharge_w*(1/discharge_efficiency-1);
    energy_rate_wh_s=(charge_efficiency*charge_w-discharge_w/discharge_efficiency)/3600;
    bus_residual_w=solar_w+discharge_w-served_w-charge_w-curtailed_w;
    return 0;
}
