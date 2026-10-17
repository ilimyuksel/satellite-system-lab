#include "orbit/include/Orbit.hh"
#include "orbit/include/Environment.hh"
#include "trick/integrator_c_intf.h"
#include "trick/exec_proto.h"
#include <cmath>
#include <stdexcept>
namespace { environment::Ephemeris ephemeris; }
int Orbit::default_data() {
    mu=3.98600435507e14;radius=6378137.0;j2=1.08262668e-3;
    use_j2=use_drag=use_srp=use_sun=use_moon=true;
    mass=12;drag_area=0.06;srp_area=0.12;cd=2.2;cr=1.3;
    rho_ref=3.725e-12;atmosphere_h_ref=400000;scale_height=58515;density_scale=1;
    earth_omega=7.292115e-5;solar_pressure=4.56e-6;epoch_offset_s=0;
    altitude0=400000;eccentricity=0;inclination_deg=51.6;raan_deg=25;
    arg_perigee_deg=0;true_anomaly_deg=0;
    initial_energy=0;perturbation_work=0;perturbation_power=0;
    return 0;
}
int Orbit::initialize() {
    for(double x:{mu,radius,j2,mass,drag_area,srp_area,cd,cr,rho_ref,density_scale,
        atmosphere_h_ref,scale_height,earth_omega,solar_pressure,epoch_offset_s,
        altitude0,eccentricity,inclination_deg,raan_deg,arg_perigee_deg,true_anomaly_deg})
        if(!std::isfinite(x))throw std::runtime_error("All model parameters must be finite");
    if(!(altitude0>0&&eccentricity>=0&&eccentricity<1&&inclination_deg>=0&&inclination_deg<=180&&
         mu>0&&radius>0&&mass>0&&drag_area>=0&&srp_area>=0&&cd>=0&&cr>=0&&cr<=2&&
         rho_ref>=0&&density_scale>=0&&scale_height>0&&solar_pressure>=0&&epoch_offset_s>=0))
        throw std::runtime_error("Invalid orbit / mass / area / atmosphere / optical parameters");
    const double pi=std::acos(-1.0),i=inclination_deg*pi/180,node=raan_deg*pi/180;
    const double arg=arg_perigee_deg*pi/180,nu=true_anomaly_deg*pi/180;
    const double rp=radius+altitude0,a=rp/(1-eccentricity),p=a*(1-eccentricity*eccentricity);
    if(use_drag&&(altitude0<150000||a*(1+eccentricity)-radius>1000000))
        throw std::runtime_error("Exponential atmosphere experiment limited to 150..1000 km; disable drag outside this range");
    const double r=p/(1+eccentricity*std::cos(nu)),v=std::sqrt(mu/p);
    // R3(RAAN) R1(i) R3(argument of perigee): perifocal -> inertial.
    const double P[3]={std::cos(node)*std::cos(arg)-std::sin(node)*std::sin(arg)*std::cos(i),
        std::sin(node)*std::cos(arg)+std::cos(node)*std::sin(arg)*std::cos(i),std::sin(arg)*std::sin(i)};
    const double Q[3]={-std::cos(node)*std::sin(arg)-std::sin(node)*std::cos(arg)*std::cos(i),
        -std::sin(node)*std::sin(arg)+std::cos(node)*std::cos(arg)*std::cos(i),std::cos(arg)*std::sin(i)};
    for(int k=0;k<3;++k){pos[k]=r*(std::cos(nu)*P[k]+std::sin(nu)*Q[k]);
        vel[k]=v*(-std::sin(nu)*P[k]+(eccentricity+std::cos(nu))*Q[k]);}
    period=2*pi*std::sqrt(a*a*a/mu);
    perturbation_work=0;
    ephemeris.load("environment/ephemeris.csv");
    evaluate_forces(0);initial_energy=energy;energy_change=energy_balance_error=0;
    return 0;
}
int Orbit::evaluate_forces(double time) {
    using namespace environment;
    V position={pos[0],pos[1],pos[2]},velocity={vel[0],vel[1],vel[2]};
    double r=norm(position),r2=r*r;
    if(!std::isfinite(r)||r<=radius)throw std::runtime_error("Earth intersection or non-finite state");
    altitude=r-radius;speed=norm(velocity);
    V central=scale(position,-mu/(r2*r)),oblateness={},dragv={},srp={},sungrav={},moongrav={},sun={},moon={};
    double k=use_j2?1.5*j2*mu*radius*radius/(r2*r2*r):0,zterm=5*pos[2]*pos[2]/r2;
    for(int axis=0;axis<3;++axis)oblateness[axis]=k*pos[axis]*(zterm-(axis==2?3:1));
    density=0;
    if(use_drag){
        if(altitude<150000||altitude>1000000)throw std::runtime_error("Outside exponential atmosphere validity experiment range (150..1000 km)");
        density=rho_ref*density_scale*std::exp(-(altitude-atmosphere_h_ref)/scale_height);
        dragv=drag(position,velocity,earth_omega,density,cd,drag_area,mass);
    }
    illumination=1;
    { // Environment is needed by EPS even when perturbing forces are disabled.
        ephemeris.at(epoch_offset_s+time,sun,moon);
        illumination=sunlight(position,sun,radius);
        sun_distance=norm(minus(sun,position));
        if(use_sun)sungrav=third_body(position,sun,1.32712440041279419e20);
        if(use_moon)moongrav=third_body(position,moon,4.902800118e12);
        if(use_srp)srp=radiation(position,sun,illumination,solar_pressure,cr,srp_area,mass);
    }
    perturbation_power=0;
    for(int axis=0;axis<3;++axis){
        central_acc[axis]=central[axis];j2_acc[axis]=oblateness[axis];drag_acc[axis]=dragv[axis];
        srp_acc[axis]=srp[axis];sun_acc[axis]=sungrav[axis];moon_acc[axis]=moongrav[axis];
        sun_pos[axis]=sun[axis];moon_pos[axis]=moon[axis];
        double external=dragv[axis]+srp[axis]+sungrav[axis]+moongrav[axis];
        acc[axis]=central[axis]+oblateness[axis]+external;
        perturbation_power+=vel[axis]*external;
    }
    central_magnitude=norm(central);j2_magnitude=norm(oblateness);drag_magnitude=norm(dragv);
    srp_magnitude=norm(srp);sun_magnitude=norm(sungrav);moon_magnitude=norm(moongrav);
    ballistic_coefficient=cd*drag_area>0?mass/(cd*drag_area):0;
    double potential=-mu/r;
    if(use_j2)potential+=mu/r*j2*radius*radius/r2*0.5*(3*pos[2]*pos[2]/r2-1);
    energy=0.5*speed*speed+potential;
    energy_change=initial_energy!=0?(energy-initial_energy)/std::abs(initial_energy):0;
    energy_balance_error=initial_energy!=0?(energy-initial_energy-perturbation_work)/std::abs(initial_energy):0;
    return 0;
}
int Orbit::derivatives(){return evaluate_forces(get_integ_time());}
int Orbit::integrate_state(){
    load_state(&pos[0],&pos[1],&pos[2],&vel[0],&vel[1],&vel[2],&perturbation_work,(double*)0);
    load_deriv(&vel[0],&vel[1],&vel[2],&acc[0],&acc[1],&acc[2],&perturbation_power,(double*)0);
    int step=integrate();
    unload_state(&pos[0],&pos[1],&pos[2],&vel[0],&vel[1],&vel[2],&perturbation_work,(double*)0);
    return step;
}
int Orbit::diagnostics(){return evaluate_forces(exec_get_sim_time());}
