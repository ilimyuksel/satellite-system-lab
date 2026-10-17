#include "system/include/Spacecraft.hh"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){
    constexpr double AU=149597870700.0;
    Thermal t;t.default_data();t.radiating_area=0;t.solar_area=0;t.earth_ir_w_m2=0;
    t.conductance_payload=0;t.initial_k[0]=310;t.initial_k[1]=280;t.initialize();
    double c0=t.mass[0]*t.cp[0],c1=t.mass[1]*t.cp[1],mean=(c0*310+c1*280)/(c0+c1);
    for(int i=0;i<1000;i++)t.advance(1);
    double delta=30*std::exp(-t.conductance_battery*(1/c0+1/c1)*1000);
    assert(std::abs(t.temperature_k[0]-(mean+c1/(c0+c1)*delta))<1e-8);
    assert(std::abs(t.temperature_k[1]-(mean-c0/(c0+c1)*delta))<1e-8);
    assert(std::abs(t.balance_error_j)<1e-6);
    t.default_data();t.radiating_area=0;t.solar_area=0;t.conductance_battery=t.conductance_payload=0;t.initialize();
    t.sources(0,AU,10,0,0,0);for(int i=0;i<100;i++)t.advance(1);
    assert(std::abs(t.temperature_k[0]-t.initial_k[0]-1000/(t.mass[0]*t.cp[0]))<1e-8);
    assert(std::abs(t.balance_error_j)<1e-6);
    t.default_data();t.initialize();double q=t.radiated_w;
    t.solar_area=0;t.earth_ir_w_m2=0;t.sources(0,AU,q,0,0,0);t.advance(1);
    assert(std::abs(t.temperature_k[0]-t.initial_k[0])<1e-10);
    t.temperature_k[1]=270;t.thermostat();assert(t.heater_requested);
    t.temperature_k[1]=280;t.thermostat();assert(t.heater_requested);
    t.temperature_k[1]=285;t.thermostat();assert(!t.heater_requested);
    Spacecraft s;s.default_data();s.eps.initial_soc=0;s.thermal.initial_k[1]=270;s.initialize(0,AU);
    assert(s.thermal.heater_requested&&s.thermal.heater_served_w==0);
    s.step(1,0,AU);assert(s.thermal.heater_energy_wh==0);
    bool rejected=false;t.default_data();t.mass[0]=0;try{t.initialize();}catch(...){rejected=true;}assert(rejected);
    std::cout<<"Thermal conduction analytic solution, isolated heating, radiation equilibrium, thermostat, unpowered heater: PASS\n";
}
