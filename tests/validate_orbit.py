#!/usr/bin/env python3
"""Independent checks on recorded Trick states. Run tests/run_validation.sh first.
SciPy DOP853 comparison requires numpy/scipy; runtime viewer uses only stdlib.
"""
import json
import math
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"tools"))
from csv_data import read
root=Path(__file__).resolve().parents[1]
base=root/"sim/SIM_satellite"

def norm(v):return math.sqrt(sum(x*x for x in v))
def cross(a,b):return [a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]]
def vec(row,name):return [row[f'{name}[{k}]'] for k in range(3)]
def load(name):
    rows=read(base/name/'log_orbit.csv')
    assert rows[-1]['time']==12000 and len(rows)==2401,'Incomplete run'
    assert all(all(math.isfinite(v) for v in r.values()) for r in rows)
    return rows

a=load('RUN_validation');b=load('RUN_j2');full=load('RUN_nominal');half=load('RUN_halfstep');drag=load('RUN_drag')
r0=vec(a[0],'pos');v0=vec(a[0],'vel');radius=norm(r0);n=math.sqrt(a[0]['mu']/radius**3)
assert a[0]['eccentricity']==0,'Analytic circular test requires circular initial elements'
max_position=max(norm([row[f'pos[{k}]']-(r0[k]*math.cos(n*row['time'])+v0[k]/n*math.sin(n*row['time'])) for k in range(3)]) for row in a)
energy_drift=max(abs(row['energy_change']) for row in b)
hz0=cross(vec(b[0],'pos'),vec(b[0],'vel'))[2]
hz_drift=max(abs((cross(vec(row,'pos'),vec(row,'vel'))[2]-hz0)/hz0) for row in b)
assert max_position<.1,f'Two-body analytic error {max_position} m'
assert energy_drift<1e-9 and hz_drift<1e-9
for rows in (a,b,full,half,drag):
    assert min(row['altitude'] for row in rows)>150000
    assert max(abs(row['energy_balance_error']) for row in rows)<1e-8

def node(row):
    h=cross(vec(row,'pos'),vec(row,'vel'));return math.atan2(h[0],-h[1])
shift=math.atan2(math.sin(node(b[-1])-node(b[0])),math.cos(node(b[-1])-node(b[0])))
rate=-1.5*n*b[0]['j2']*(b[0]['radius']/radius)**2*math.cos(math.radians(b[0]['inclination_deg']))
assert abs(shift/(rate*b[-1]['time'])-1)<.15
assert drag[-1]['energy']<drag[0]['energy']
assert drag[-1]['perturbation_work']<0
assert min(r['illumination'] for r in full)==0 and max(r['illumination'] for r in full)==1
assert any(0<r['illumination']<1 for r in full),'Penumbra not sampled'
for r in full:
    if r['illumination']==0:assert r['srp_magnitude']==0
    for k in range(3):
        assert math.isfinite(r[f'pos[{k}]'])
# Step refinement checks all perturbations and their timing in the Trick loop.
step_difference=max(norm([r[f'pos[{k}]']-s[f'pos[{k}]'] for k in range(3)]) for r,s in zip(full,half))
assert step_difference<.5,f'1 s vs 0.5 s difference {step_difference} m'
report=dict(duration_s=12000,two_body_max_position_error_m=max_position,
    j2_relative_energy_drift=energy_drift,j2_relative_axial_momentum_drift=hz_drift,
    j2_node_shift_deg=math.degrees(shift),full_energy_balance_residual=max(abs(r['energy_balance_error']) for r in full),
    drag_specific_energy_change_m2_s2=drag[-1]['energy']-drag[0]['energy'],
    full_versus_halfstep_max_position_difference_m=step_difference)

# Independent Python force implementation and DOP853 integrator: helps detect Trick
# scheduling/stage-time mistakes. Shares physical constants and JPL data by design.
import numpy as np
from scipy.integrate import solve_ivp
from scipy.interpolate import CubicHermiteSpline
p=full[0]
eph=np.loadtxt(base/'environment/ephemeris.csv',delimiter=',',skiprows=1)
sun_spline=CubicHermiteSpline(eph[:,0],eph[:,1:4],eph[:,4:7]);moon_spline=CubicHermiteSpline(eph[:,0],eph[:,7:10],eph[:,10:13])
def reference(t,state):
    r=state[:3];v=state[3:];rnorm=np.linalg.norm(r);mu=p['mu'];R=p['radius'];z=r[2]/rnorm
    acc=-mu*r/rnorm**3
    if p['use_j2']:acc+=1.5*p['j2']*mu*R**2/rnorm**5*r*np.array([5*z*z-1,5*z*z-1,5*z*z-3])
    sun=sun_spline(t+p['epoch_offset_s']);moon=moon_spline(t+p['epoch_offset_s'])
    if p['use_drag']:
        relative=v-np.cross([0,0,p['earth_omega']],r)
        density=p['rho_ref']*p['density_scale']*np.exp(-(rnorm-R-p['atmosphere_h_ref'])/p['scale_height'])
        acc-=.5*density*p['cd']*p['drag_area']/p['mass']*np.linalg.norm(relative)*relative
    for body,gm,enabled in ((sun,1.32712440041279419e20,p['use_sun']),(moon,4.902800118e12,p['use_moon'])):
        if enabled:
            d=body-r;acc+=gm*(d/np.linalg.norm(d)**3-body/np.linalg.norm(body)**3)
    if p['use_srp']:
        tosun=sun-r;d=np.linalg.norm(tosun);alpha=np.arcsin(695700000/d);beta=np.arcsin(R/rnorm)
        theta=np.arccos(np.clip(np.dot(-r,tosun)/(rnorm*d),-1,1));lit=1.
        if theta<=beta-alpha:lit=0.
        elif theta<alpha+beta:
            if theta<=alpha-beta:lit=1-(beta/alpha)**2
            else:
                # Standard two-circle overlap, independently expressed.
                x=(theta**2+alpha**2-beta**2)/(2*theta*alpha)
                y=(theta**2+beta**2-alpha**2)/(2*theta*beta)
                term=(-theta+alpha+beta)*(theta+alpha-beta)*(theta-alpha+beta)*(theta+alpha+beta)
                overlap=alpha**2*np.arccos(np.clip(x,-1,1))+beta**2*np.arccos(np.clip(y,-1,1))-.5*np.sqrt(max(0,term))
                lit=np.clip(1-overlap/(np.pi*alpha**2),0,1)
        acc-=lit*p['solar_pressure']*p['cr']*p['srp_area']/p['mass']*(149597870700/d)**2*tosun/d
    return np.r_[v,acc]
times=np.array([r['time'] for r in full]);state=np.array(vec(p,'pos')+vec(p,'vel'))
ref=solve_ivp(reference,(0,12000),state,method='DOP853',rtol=2e-12,atol=1e-7,t_eval=times,max_step=20)
assert ref.success
actual=np.array([vec(r,'pos') for r in full])
reference_error=float(np.max(np.linalg.norm(actual-ref.y[:3].T,axis=1)))
assert reference_error<1.0,f'DOP853 comparison error {reference_error} m'
report.update(dop853_max_position_difference_m=reference_error,status='PASS')
(root/'artifacts/orbit-validation.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
