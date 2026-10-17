#!/usr/bin/env python3
import sys,json,math
from pathlib import Path
root=Path(__file__).resolve().parents[1];sys.path.insert(0,str(root/'tools'))
from csv_data import read
key=lambda r,k:r['dyn.spacecraft.thermal.'+k]
report={};data={}
for scenario in ('nominal','low_power','cold','cold_no_heater'):
    rows=read(root/'sim/SIM_satellite'/f'RUN_{scenario}/log_orbit.csv');data[scenario]=rows
    assert len(rows)==2401 and rows[-1]['time']==12000
    assert all(math.isfinite(key(r,f'temperature_k[{i}]')) and 100<key(r,f'temperature_k[{i}]')<500 for r in rows for i in range(3))
    residual=max(abs(key(r,'balance_error_j')) for r in rows);assert residual<1e-5
    assert all(key(r,'heater_served_w')<=key(r,'heater_rating_w')+1e-9 for r in rows)
    assert all(key(r,'heater_served_w')==0 for r in rows if not key(r,'heater_requested'))
    assert all(key(r,'solar_absorbed_w')==0 for r in rows if r['illumination']==0)
    for r in rows:
        served=r['dyn.spacecraft.eps.served_w'];payload=r['dyn.spacecraft.payload.consumed_w']
        base=min(r['dyn.spacecraft.eps.base_load_w'],served)
        assert abs(served-base-payload-key(r,'heater_served_w'))<1e-8
        assert abs(key(r,'internal_w[1]')-r['dyn.spacecraft.eps.ohmic_heat_w']-key(r,'heater_served_w'))<1e-8
    report[scenario]={'final_c':[key(rows[-1],f'temperature_c[{i}]') for i in range(3)],
        'min_c':[min(key(r,f'temperature_c[{i}]') for r in rows) for i in range(3)],
        'max_c':[max(key(r,f'temperature_c[{i}]') for r in rows) for i in range(3)],
        'thermal_balance_max_error_j':residual,'heater_energy_wh':key(rows[-1],'heater_energy_wh')}
assert report['cold']['heater_energy_wh']>0 and report['cold_no_heater']['heater_energy_wh']==0
assert key(data['cold'][120],'temperature_k[1]')>key(data['cold_no_heater'][120],'temperature_k[1]')
assert data['cold'][120]['dyn.spacecraft.eps.soc']<data['cold_no_heater'][120]['dyn.spacecraft.eps.soc']
report['status']='PASS';(root/'artifacts/thermal-validation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
