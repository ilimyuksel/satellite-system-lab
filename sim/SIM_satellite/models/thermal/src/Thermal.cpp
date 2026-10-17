#include "thermal/include/Thermal.hh"
#include <cmath>
#include <stdexcept>
#include <initializer_list>
namespace {
constexpr double sigma=5.670374419e-8;
void rate(const Thermal& model,const double* t,double* out,double& total){
    double radiation=sigma*model.emissivity*model.radiating_area*(std::pow(t[0],4)-std::pow(model.space_k,4));
    double qb=model.conductance_battery*(t[0]-t[1]);
    double qp=model.conductance_payload*(t[0]-t[2]);
    double q[3]={model.solar_absorbed_w+model.earth_absorbed_w+model.internal_w[0]-radiation-qb-qp,
                 model.internal_w[1]+qb,model.internal_w[2]+qp};
    total=q[0]+q[1]+q[2];
    for(int i=0;i<3;++i)out[i]=q[i]/(model.mass[i]*model.cp[i]);
}
}
int Thermal::default_data(){
    mass[0]=8;mass[1]=2;mass[2]=2;cp[0]=900;cp[1]=1000;cp[2]=800;
    for(int i=0;i<3;++i)initial_k[i]=293.15;
    conductance_battery=.6;conductance_payload=.8;solar_area=.06;absorptivity=.6;
    radiating_area=.25;emissivity=.75;earth_ir_w_m2=237;earth_view_factor=.3;space_k=3;
    heater_enabled=true;heater_on_k=278.15;heater_off_k=283.15;heater_rating_w=8;
    return 0;
}
int Thermal::initialize(){
    for(int i=0;i<3;++i){
        if(!(std::isfinite(mass[i])&&std::isfinite(cp[i])&&std::isfinite(initial_k[i])&&mass[i]>0&&cp[i]>0&&initial_k[i]>0))
            throw std::runtime_error("Invalid thermal mass, heat capacity or initial Kelvin temperature");
        temperature_k[i]=initial_k[i];internal_w[i]=0;
    }
    for(double x:{conductance_battery,conductance_payload,solar_area,absorptivity,radiating_area,emissivity,
        earth_ir_w_m2,earth_view_factor,space_k,heater_on_k,heater_off_k,heater_rating_w})
        if(!std::isfinite(x))throw std::runtime_error("Non-finite thermal parameter");
    if(!(conductance_battery>=0&&conductance_payload>=0&&solar_area>=0&&absorptivity>=0&&absorptivity<=1&&
        radiating_area>=0&&emissivity>=0&&emissivity<=1&&earth_ir_w_m2>=0&&earth_view_factor>=0&&earth_view_factor<=1&&
        space_k>=0&&heater_on_k>0&&heater_off_k>heater_on_k&&heater_rating_w>=0))throw std::runtime_error("Invalid thermal environment or thermostat");
    heater_requested=false;heater_served_w=solar_absorbed_w=earth_absorbed_w=net_energy_j=balance_error_j=heater_energy_wh=0;
    thermostat();return diagnostics();
}
int Thermal::advance(double dt){
    if(!std::isfinite(dt)||dt<0||dt>1.000001)throw std::runtime_error("Thermal RK4 interval must be 0..1 s");
    // Hold external/electrical inputs over the subsystem interval; evaluate radiation/conduction at RK stages.
    double k[4][3],q[4],stage[3];rate(*this,temperature_k,k[0],q[0]);
    for(int j=1;j<4;++j){
        for(int i=0;i<3;++i)stage[i]=temperature_k[i]+dt*(j==3?1:.5)*k[j-1][i];
        rate(*this,stage,k[j],q[j]);
    }
    for(int i=0;i<3;++i){
        temperature_k[i]+=dt*(k[0][i]+2*k[1][i]+2*k[2][i]+k[3][i])/6;
        if(!std::isfinite(temperature_k[i])||temperature_k[i]<=0)throw std::runtime_error("Invalid thermal state: check timestep and parameters");
    }
    net_energy_j+=dt*(q[0]+2*q[1]+2*q[2]+q[3])/6;
    heater_energy_wh+=heater_served_w*dt/3600;
    return diagnostics();
}
int Thermal::thermostat(){
    if(!heater_enabled)heater_requested=false;
    else if(temperature_k[1]<=heater_on_k)heater_requested=true;
    else if(temperature_k[1]>=heater_off_k)heater_requested=false;
    return 0;
}
int Thermal::sources(double visibility,double distance,double body_w,double battery_w,double payload_w,double heater_w){
    for(double x:{visibility,distance,body_w,battery_w,payload_w,heater_w})if(!std::isfinite(x))throw std::runtime_error("Non-finite heat source");
    if(visibility<0||visibility>1||distance<=0||body_w<0||battery_w<0||payload_w<0||heater_w<0)throw std::runtime_error("Invalid heat source");
    const double au=149597870700.0;
    solar_absorbed_w=1361*std::pow(au/distance,2)*solar_area*absorptivity*visibility;
    earth_absorbed_w=earth_ir_w_m2*earth_view_factor*radiating_area*emissivity;
    internal_w[0]=body_w;internal_w[1]=battery_w+heater_w;internal_w[2]=payload_w;heater_served_w=heater_w;
    return diagnostics();
}
int Thermal::diagnostics(){
    double stored=0;for(int i=0;i<3;++i){stored+=mass[i]*cp[i]*(temperature_k[i]-initial_k[i]);temperature_c[i]=temperature_k[i]-273.15;}
    balance_error_j=stored-net_energy_j;
    radiated_w=sigma*emissivity*radiating_area*(std::pow(temperature_k[0],4)-std::pow(space_k,4));
    body_to_battery_w=conductance_battery*(temperature_k[0]-temperature_k[1]);
    body_to_payload_w=conductance_payload*(temperature_k[0]-temperature_k[2]);
    return 0;
}
