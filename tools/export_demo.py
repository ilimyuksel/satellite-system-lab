#!/usr/bin/env python3
"""Embed recorded Trick state and force diagnostics into the offline viewer."""
import json
from pathlib import Path
import sys
from csv_data import read
base=Path(__file__).resolve().parents[1]
scenario=sys.argv[1] if len(sys.argv)>1 else 'nominal'
if scenario not in ('nominal','low_power'): raise SystemExit('Unknown scenario')
log=base/'artifacts'/f'{scenario}.log'
if 'Reached termination time' not in log.read_text(): raise SystemExit('Simulation did not complete normally')
rows=read(base/'sim/SIM_satellite'/f'RUN_{scenario}'/'log_orbit.csv')
if rows[-1]['time'] != 12000: raise SystemExit('Expected complete 12000 second demo')
first=rows[0]
fields=['time']+[f'{v}[{k}]' for v in ('pos','vel') for k in range(3)]+[
    'altitude','speed','energy_change','energy_balance_error','density','illumination',
    'central_magnitude','j2_magnitude','drag_magnitude','srp_magnitude','sun_magnitude','moon_magnitude',
    'dyn.spacecraft.eps.soc','dyn.spacecraft.eps.solar_w','dyn.spacecraft.eps.served_w',
    'dyn.spacecraft.eps.energy_wh','dyn.spacecraft.payload.mode','dyn.spacecraft.low_power',
    'dyn.spacecraft.eps.balance_error_wh','dyn.spacecraft.payload.commanded']
config=['mass','drag_area','srp_area','cd','cr','rho_ref','scale_height','density_scale','epoch_offset_s',
        'use_j2','use_drag','use_srp','use_sun','use_moon','ballistic_coefficient',
        'altitude0','eccentricity','raan_deg','arg_perigee_deg','true_anomaly_deg']
data=dict(scenario=scenario, schema_version=1, radius=first['radius'],mu=first['mu'],j2=bool(first['use_j2']),
    inclination=first['inclination_deg'],config={k:first[k] for k in config},
    epoch='2026-10-16 00:00:00 TDB',
    rows=[[float(format(row[k],'.12g')) for k in fields] for row in rows])
text=(base/'web/viewer.template.html').read_text()
(base/'web/index.html').write_text(text.replace('__ORBIT_DATA__',json.dumps(data,separators=(',',':'),allow_nan=False)))
print('3D viewer:',base/'web/index.html')

summary={"scenario":scenario,"duration_s":rows[-1]['time'],"record_count":len(rows),
 "soc_initial":first['dyn.spacecraft.eps.soc'],"soc_final":rows[-1]['dyn.spacecraft.eps.soc'],
 "payload_active_seconds":rows[-1]['dyn.spacecraft.payload.active_seconds'],
 "low_power_records":sum(bool(r['dyn.spacecraft.low_power']) for r in rows),
 "maximum_eps_balance_error_wh":max(abs(r['dyn.spacecraft.eps.balance_error_wh']) for r in rows)}
(base/'artifacts'/f'{scenario}_summary.json').write_text(json.dumps(summary,indent=2)+'\n')
(base/'web/demo-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
