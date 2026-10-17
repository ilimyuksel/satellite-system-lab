#include "orbit/include/Environment.hh"
#include <cassert>
#include <iostream>
int main(){
    using namespace environment;
    constexpr double R=6378137,AU=149597870700.0;
    V sun={AU,0,0},day={R+400000,0,0},night={-R-400000,0,0};
    assert(sunlight(day,sun,R)==1);assert(sunlight(night,sun,R)==0);
    assert(norm(radiation(night,sun,0,4.56e-6,1.3,.12,12))==0);
    auto srp=radiation(day,sun,1,4.56e-6,1.3,.12,12);
    assert(srp[0]<0&&srp[1]==0&&srp[2]==0);
    auto heavy=radiation(day,sun,1,4.56e-6,1.3,.12,24);
    assert(std::abs(norm(heavy)/norm(srp)-.5)<1e-14);
    auto wide=radiation(day,sun,1,4.56e-6,1.3,.24,12);
    assert(std::abs(norm(wide)/norm(srp)-2)<1e-14);
    V velocity={0,7700,0};
    auto d=drag(day,velocity,7.292115e-5,3.725e-12,2.2,.06,12);
    assert(dot(d,velocity)<0);
    assert(std::abs(norm(drag(day,velocity,7.292115e-5,3.725e-12,2.2,.06,24))/norm(d)-.5)<1e-14);
    assert(norm(drag(day,{0,7.292115e-5*day[0],0},7.292115e-5,3.725e-12,2.2,.06,12))<1e-30);
    assert(norm(third_body({0,0,0},sun,1.327e20))==0);
    // Collinear solar tide stretches the radial direction; signs test indirect term.
    assert(third_body(day,sun,1.327e20)[0]>0);
    bool partial=false;
    for(int i=0;i<10000;++i){double a=1.0+i*.0001;V p={-(R+400000)*std::cos(a),(R+400000)*std::sin(a),0};
        double lit=sunlight(p,sun,R);assert(lit>=0&&lit<=1);if(lit>0&&lit<1)partial=true;}
    assert(partial);
    Ephemeris eph;eph.load("environment/ephemeris.csv");V a,b,c,e;
    eph.at(0,a,b);assert(norm(a)>1.4e11&&norm(a)<1.6e11);assert(norm(b)>3.0e8&&norm(b)<4.2e8);
    eph.at(300,c,e);eph.at(150,a,b);assert(norm(a)>1.4e11);
    bool rejected=false;try{eph.at(172801,a,b);}catch(const std::runtime_error&){rejected=true;}assert(rejected);
    std::cout<<"Force direction, mass/area scaling, corotation, eclipse, ephemeris range: PASS\n";
}
