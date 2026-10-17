#ifndef LEARNING_ENVIRONMENT_HH
#define LEARNING_ENVIRONMENT_HH
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace environment {
using V = std::array<double,3>;
inline double dot(const V&a,const V&b){return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}
inline double norm(const V&a){return std::sqrt(dot(a,a));}
inline V minus(const V&a,const V&b){return {a[0]-b[0],a[1]-b[1],a[2]-b[2]};}
inline V scale(const V&a,double s){return {a[0]*s,a[1]*s,a[2]*s};}
inline V third_body(const V&position,const V&body,double gm){
    V d=minus(body,position); double rn=norm(body),dn=norm(d);
    if(rn<=0 || dn<=0) throw std::runtime_error("Singular third-body position");
    // Geocentric frame: subtract Earth's acceleration toward the same body.
    return minus(scale(d,gm/(dn*dn*dn)),scale(body,gm/(rn*rn*rn)));
}
inline V drag(const V&position,const V&velocity,double omega,double rho,double cd,double area,double mass){
    V relative={velocity[0]+omega*position[1],velocity[1]-omega*position[0],velocity[2]};
    return scale(relative,-0.5*rho*cd*area/mass*norm(relative));
}
inline double sunlight(const V&position,const V&sun,double earth_radius){
    // Apparent disk overlap, with spherical Earth and uniform solar disk.
    const double solar_radius=695700000.0, pi=std::acos(-1.0);
    V tosun=minus(sun,position), toearth=scale(position,-1);
    double r=norm(position),d=norm(tosun);
    if(r<=earth_radius || d<=solar_radius) throw std::runtime_error("Invalid eclipse geometry");
    double a=std::asin(solar_radius/d),b=std::asin(earth_radius/r);
    double c=std::acos(std::clamp(dot(tosun,toearth)/(d*r),-1.0,1.0));
    if(c>=a+b) return 1;
    if(b>=c+a) return 0;
    if(a>=c+b) return std::clamp(1-b*b/(a*a),0.0,1.0);
    double x=(c*c+a*a-b*b)/(2*c),y=std::sqrt(std::max(0.0,a*a-x*x));
    double overlap=a*a*std::acos(std::clamp(x/a,-1.0,1.0))+
        b*b*std::acos(std::clamp((c-x)/b,-1.0,1.0))-c*y;
    return std::clamp(1-overlap/(pi*a*a),0.0,1.0);
}
inline V radiation(const V&position,const V&sun,double lit,double pressure,double cr,double area,double mass){
    constexpr double au=149597870700.0;
    V away=minus(position,sun);double d=norm(away);
    return scale(away,lit*pressure*cr*area/mass*(au/d)*(au/d)/d);
}
class Ephemeris {
    std::vector<std::array<double,13>> rows;
public:
    void load(const std::string&filename){
        rows.clear();std::ifstream f(filename);std::string line;
        if(!f) throw std::runtime_error("Missing environment/ephemeris.csv; see README");
        std::getline(f,line);
        while(std::getline(f,line)){
            if(line.empty())continue;
            std::replace(line.begin(),line.end(),',',' ');std::istringstream s(line);
            std::array<double,13> r;
            for(double &x:r) if(!(s>>x)||!std::isfinite(x)) throw std::runtime_error("Invalid ephemeris row");
            if(!rows.empty()&&r[0]<=rows.back()[0])throw std::runtime_error("Ephemeris times must increase");
            rows.push_back(r);
        }
        if(rows.size()<2)throw std::runtime_error("Incomplete ephemeris");
    }
    void at(double t,V&sun,V&moon)const{
        if(rows.size()<2||!std::isfinite(t)||t<rows.front()[0]||t>rows.back()[0])
            throw std::runtime_error("Epoch/time outside bundled JPL ephemeris coverage (0..172800 s)");
        auto it=std::upper_bound(rows.begin(),rows.end(),t,[](double x,const auto&r){return x<r[0];});
        size_t j=it==rows.end()?rows.size()-1:it-rows.begin();
        const auto&a=rows[j-1];const auto&b=rows[j];double dt=b[0]-a[0],u=(t-a[0])/dt;
        double h00=2*u*u*u-3*u*u+1,h10=u*u*u-2*u*u+u,h01=-2*u*u*u+3*u*u,h11=u*u*u-u*u;
        for(int k=0;k<3;++k){
            sun[k]=h00*a[k+1]+h10*dt*a[k+4]+h01*b[k+1]+h11*dt*b[k+4];
            moon[k]=h00*a[k+7]+h10*dt*a[k+10]+h01*b[k+7]+h11*dt*b[k+10];
        }
    }
};
}
#endif
