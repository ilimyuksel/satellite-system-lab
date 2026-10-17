#!/usr/bin/env python3
import json
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from csv_data import read
root=Path(__file__).resolve().parents[1]
report={}
for scenario in ('nominal','low_power'):
    rows=read(root/'sim/SIM_satellite'/f'RUN_{scenario}'/'log_orbit.csv')
    key=lambda r,k:r['dyn.spacecraft.'+k]
    assert len(rows)==2401 and rows[-1]['time']==12000
    assert all(0<=key(r,'eps.soc')<=1 for r in rows)
    assert all(key(r,'payload.mode') in (0,1,2) for r in rows)
    assert key(rows[0],'payload.active_seconds')==0
    assert key(rows[0],'payload.heat_fraction')<=1
    error=max(abs(key(r,'eps.balance_error_wh')) for r in rows)
    assert error<1e-8
    assert max(abs(key(r,'eps.bus_residual_w')) for r in rows)<1e-9
    assert all(key(r,'eps.solar_w')==0 for r in rows if r['illumination']==0)
    assert any(r['illumination']==0 for r in rows) and any(r['illumination']==1 for r in rows)
    assert all(key(r,'payload.mode')!=2 for r in rows if key(r,'low_power'))
    if scenario=='nominal':assert any(key(r,'payload.mode')==2 for r in rows)
    assert key(rows[0],'eps.energy_wh')==key(rows[0],'eps.initial_soc')*key(rows[0],'eps.effective_capacity_wh')
    assert all(-key(r,'eps.max_charge_a')-1e-9<=key(r,'eps.battery_current_a')<=key(r,'eps.max_discharge_a')+1e-9 for r in rows)
    assert all(key(r,'eps.ohmic_heat_w')>=0 for r in rows)
    assert abs(key(rows[-1],'payload.generated_data_mbit')-key(rows[-1],'payload.active_seconds')*key(rows[0],'payload.data_rate_mbps'))<1e-6
    if scenario=='low_power':assert any(key(r,'power_limited') for r in rows)
    report[scenario]={'soc_initial':key(rows[0],'eps.soc'),'soc_final':key(rows[-1],'eps.soc'),
        'payload_active_s':key(rows[-1],'payload.active_seconds'),'energy_balance_max_error_wh':error,
        'low_power_records':sum(bool(key(r,'low_power')) for r in rows)}
report['status']='PASS'
(root/'artifacts/system-validation.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
