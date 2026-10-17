#!/usr/bin/env python3
"""Embed recorded Trick state and force diagnostics into the offline viewer."""
import json
from pathlib import Path
import sys
from csv_data import read
base=Path(__file__).resolve().parents[1]
scenario=sys.argv[1] if len(sys.argv)>1 else 'nominal'
if scenario not in ('nominal','low_power','cold','cold_no_heater'): raise SystemExit('Unknown scenario')
log=base/'artifacts'/f'{scenario}.log'
if 'Reached termination time' not in log.read_text(): raise SystemExit('Simulation did not complete normally')
rows=read(base/'sim/SIM_satellite'/f'RUN_{scenario}'/'log_orbit.csv')
if rows[-1]['time'] <= 0 or any(b['time']<=a['time'] for a,b in zip(rows,rows[1:])):
    raise SystemExit('Expected increasing telemetry times')
first=rows[0]
fields=['time']+[f'{v}[{k}]' for v in ('pos','vel') for k in range(3)]+[
    'altitude','speed','energy_change','energy_balance_error','density','illumination',
    'central_magnitude','j2_magnitude','drag_magnitude','srp_magnitude','sun_magnitude','moon_magnitude',
    'dyn.spacecraft.eps.soc','dyn.spacecraft.eps.solar_w','dyn.spacecraft.eps.served_w',
    'dyn.spacecraft.eps.energy_wh','dyn.spacecraft.payload.mode','dyn.spacecraft.low_power',
    'dyn.spacecraft.eps.balance_error_wh','dyn.spacecraft.payload.commanded',
    'sun_pos[0]','sun_pos[1]','sun_pos[2]',
    'dyn.spacecraft.eps.charge_w','dyn.spacecraft.eps.discharge_w',
    'dyn.spacecraft.eps.terminal_v','dyn.spacecraft.eps.battery_current_a',
    'dyn.spacecraft.eps.ohmic_heat_w','dyn.spacecraft.payload.generated_data_mbit','dyn.spacecraft.payload.heat_w',
    'dyn.spacecraft.thermal.temperature_c[0]','dyn.spacecraft.thermal.temperature_c[1]',
    'dyn.spacecraft.thermal.temperature_c[2]','dyn.spacecraft.thermal.heater_served_w',
    'dyn.spacecraft.thermal.heater_requested','dyn.spacecraft.thermal.balance_error_j']
config=['mass','drag_area','srp_area','cd','cr','rho_ref','scale_height','density_scale','epoch_offset_s',
        'use_j2','use_drag','use_srp','use_sun','use_moon','ballistic_coefficient',
        'altitude0','eccentricity','raan_deg','arg_perigee_deg','true_anomaly_deg']
parameter_names=['capacity_wh','capacity_soh','initial_soc','panel_area','panel_efficiency','incidence_cosine',
    'panel_health','regulation_efficiency','charge_efficiency','discharge_efficiency','ocv_empty_v','ocv_full_v',
    'internal_resistance','min_terminal_v','max_terminal_v','max_charge_a','max_discharge_a','max_charge_w',
    'max_discharge_w','base_load_w','cutoff_soc','recovery_soc']
parameter_snapshot={'eps.'+k:first['dyn.spacecraft.eps.'+k] for k in parameter_names}
for k in ['active_w','standby_w','first_on_s','cycle_s','on_duration_s','data_rate_mbps','heat_fraction']:
    parameter_snapshot['payload.'+k]=first['dyn.spacecraft.payload.'+k]
for k in ['conductance_battery','conductance_payload','solar_area','absorptivity','radiating_area','emissivity',
          'earth_ir_w_m2','earth_view_factor','space_k','heater_enabled','heater_on_k','heater_off_k','heater_rating_w']:
    parameter_snapshot['thermal.'+k]=first['dyn.spacecraft.thermal.'+k]
for k in ['mass','cp','initial_k']:
    for i in range(3):parameter_snapshot[f'thermal.{k}[{i}]']=first[f'dyn.spacecraft.thermal.{k}[{i}]']
data=dict(parameters=parameter_snapshot, scenario=scenario, schema_version=4, radius=first['radius'],mu=first['mu'],j2=bool(first['use_j2']),
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
