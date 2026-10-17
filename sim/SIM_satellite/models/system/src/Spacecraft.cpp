#include "system/include/Spacecraft.hh"
#include <algorithm>
#include <cmath>
#include <stdexcept>
int Spacecraft::default_data(){eps.default_data();payload.default_data();thermal.default_data();return 0;}
int Spacecraft::initialize(double visibility,double sun_distance){
    eps.initialize();payload.initialize();thermal.initialize();low_power=eps.soc<=eps.cutoff_soc;last_time=0;
    return step(0,visibility,sun_distance);
}
int Spacecraft::step(double time,double visibility,double sun_distance){
    double dt=time-last_time;
    if(dt<0||dt>1.000001)throw std::runtime_error("Subsystem job requires an interval no greater than 1 second");
    // Advance using previous interval's held powers before computing the next interval.
    if(payload.mode==2){payload.active_seconds+=dt;payload.generated_data_mbit+=payload.data_rate_mbps*dt;}
    eps.advance(dt);thermal.advance(dt);last_time=time;
    thermal.thermostat();
    if(eps.soc<=eps.cutoff_soc)low_power=true;
    else if(eps.soc>=eps.recovery_soc)low_power=false;
    payload.command(time);eps.solar(visibility,sun_distance);
    const double available=eps.available(1.0);
    const double heater_request=thermal.heater_requested?thermal.heater_rating_w:0;
    const double priority=eps.base_load_w+heater_request;
    payload.mode=payload.commanded&&!low_power&&available>=priority+payload.active_w?2:1;
    if(available<priority+payload.standby_w)payload.mode=0;
    double demand=priority+(payload.mode==2?payload.active_w:payload.mode==1?payload.standby_w:0);
    eps.allocate(priority+payload.requested_w,demand,1.0);
    const double base_served=std::min(eps.base_load_w,eps.served_w);
    const double heater_served=std::min(heater_request,std::max(0.0,eps.served_w-base_served));
    payload.consumed_w=std::max(0.0,eps.served_w-base_served-heater_served);
    payload.heat_w=payload.consumed_w*payload.heat_fraction;
    thermal.sources(visibility,sun_distance,base_served+eps.converter_loss_w,eps.ohmic_heat_w,payload.heat_w,heater_served);
    power_limited=payload.commanded&&payload.mode!=2;
    return 0;
}
