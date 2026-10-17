#include "payload/include/Payload.hh"
#include <cmath>
#include <stdexcept>
#include <initializer_list>
int Payload::default_data(){active_w=28;standby_w=2;first_on_s=600;cycle_s=1800;on_duration_s=900;data_rate_mbps=2;heat_fraction=.9;return 0;}
int Payload::initialize(){
    for(double x:{active_w,standby_w,first_on_s,cycle_s,on_duration_s,data_rate_mbps,heat_fraction})if(!std::isfinite(x))throw std::runtime_error("Non-finite payload parameter");
    if(!(active_w>=standby_w&&standby_w>=0&&first_on_s>=0&&cycle_s>0&&on_duration_s>=0&&on_duration_s<=cycle_s&&data_rate_mbps>=0&&heat_fraction>=0&&heat_fraction<=1))
        throw std::runtime_error("Invalid payload schedule or power demand");
    commanded=false;mode=1;requested_w=standby_w;consumed_w=0;active_seconds=0;generated_data_mbit=0;heat_w=0;return 0;
}
int Payload::command(double time){commanded=time>=first_on_s&&std::fmod(time-first_on_s,cycle_s)<on_duration_s;requested_w=commanded?active_w:standby_w;return 0;}
