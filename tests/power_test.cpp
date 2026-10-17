#include "system/include/Spacecraft.hh"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){
    constexpr double AU=149597870700.0;
    Power p;p.default_data();p.internal_resistance=0;p.initialize();
    p.solar(0,AU);p.allocate(10,10,1);double before=p.energy_wh;
    p.advance(1);assert(std::abs(p.energy_wh-(before-10/.95/3600))<1e-12);
    assert(std::abs(p.balance_error_wh)<1e-10);
    p.default_data();p.initial_soc=1;p.initialize();p.solar(1,AU);p.allocate(0,0,1);
    assert(p.charge_w==0&&p.curtailed_w>0);p.advance(1);assert(p.soc==1);
    p.default_data();p.initial_soc=0;p.initialize();p.solar(0,AU);p.allocate(10,10,1);
    assert(p.served_w==0&&p.unserved_w==10);p.advance(1);assert(p.soc==0);
    p.default_data();p.panel_area=0;p.initialize();p.solar(1,AU);assert(p.solar_w==0);
    bool rejected=false;
    p.default_data();p.charge_efficiency=0;try{p.initialize();}catch(...){rejected=true;}assert(rejected);
    // Positive resistance changes terminal voltage, dissipates heat, and limits current.
    p.default_data();p.max_discharge_a=.1;p.initialize();p.solar(0,AU);p.allocate(40,40,1);
    assert(p.battery_current_a<=.1+1e-12&&p.unserved_w>0&&p.terminal_v<p.ocv_v&&p.ohmic_heat_w>0);
    p.advance(1);assert(std::abs(p.balance_error_wh)<1e-10);
    p.default_data();p.capacity_soh=.7;p.initialize();assert(std::abs(p.energy_wh-80*.7*.65)<1e-10);
    p.default_data();p.max_charge_a=.1;p.initial_soc=.5;p.initialize();p.solar(1,AU);p.allocate(0,0,1);
    assert(-p.battery_current_a<=.1+1e-12&&p.terminal_v<=p.max_terminal_v+1e-12);
    p.advance(1);assert(std::abs(p.balance_error_wh)<1e-10);
    p.default_data();p.min_terminal_v=16.5;p.initialize();p.solar(0,AU);p.allocate(10,10,1);assert(p.discharge_w==0);
    Spacecraft s;s.default_data();s.thermal.heater_enabled=false;s.eps.initial_soc=.22;s.payload.first_on_s=0;
    s.payload.on_duration_s=s.payload.cycle_s=20000;s.initialize(0,AU);
    assert(s.payload.mode==2);
    for(int t=1;t<=1000;++t)s.step(t,0,AU);
    assert(s.low_power&&s.payload.mode==1&&s.power_limited);
    for(int t=1001;t<=8000;++t)s.step(t,1,AU);
    assert(!s.low_power&&s.payload.mode==2);
    assert(s.eps.soc>=0&&s.eps.soc<=1&&std::abs(s.eps.balance_error_wh)<1e-9);
    assert(std::abs(s.eps.bus_residual_w)<1e-10);
    assert(std::abs(s.payload.generated_data_mbit-s.payload.active_seconds*s.payload.data_rate_mbps)<1e-9);
    assert(s.payload.heat_w==s.payload.consumed_w*s.payload.heat_fraction);
    std::cout<<"EPS energy balance, saturation, empty storage, invalid parameters, shedding/recovery: PASS\n";
}
