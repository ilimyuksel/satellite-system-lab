#include "system/include/Spacecraft.hh"
#include <algorithm>
#include <cmath>
#include <stdexcept>
int Spacecraft::default_data(){eps.default_data();payload.default_data();return 0;}
int Spacecraft::initialize(double visibility,double sun_distance){
    eps.initialize();payload.initialize();low_power=eps.soc<=eps.cutoff_soc;last_time=0;
    return step(0,visibility,sun_distance);
}
int Spacecraft::step(double time,double visibility,double sun_distance){
    double dt=time-last_time;
    if(dt<0||dt>1.000001)throw std::runtime_error("Subsystem job requires an interval no greater than 1 second");
    // Advance using previous interval's held powers before computing the next interval.
    if(payload.mode==2)payload.active_seconds+=dt;
    eps.advance(dt);last_time=time;
    if(eps.soc<=eps.cutoff_soc)low_power=true;
    else if(eps.soc>=eps.recovery_soc)low_power=false;
    payload.command(time);eps.solar(visibility,sun_distance);
    const double available=eps.available(1.0);
    payload.mode=payload.commanded&&!low_power&&available>=eps.base_load_w+payload.active_w?2:1;
    if(available<eps.base_load_w+payload.standby_w)payload.mode=0;
    double demand=eps.base_load_w+(payload.mode==2?payload.active_w:payload.mode==1?payload.standby_w:0);
    eps.allocate(eps.base_load_w+payload.requested_w,demand,1.0);
    payload.consumed_w=std::max(0.0,eps.served_w-eps.base_load_w);
    power_limited=payload.commanded&&payload.mode!=2;
    return 0;
}
