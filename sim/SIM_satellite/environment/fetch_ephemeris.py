#!/usr/bin/env python3
"""Fetch reproducible geometric geocentric ICRF Sun/Moon states from JPL Horizons.
Only used when refreshing the bundled data; normal simulation runs are offline.
Epoch and table range are TDB, not UTC. No user data is transmitted.
"""
import csv
import io
import json
from pathlib import Path
import urllib.parse
import urllib.request

base = Path(__file__).resolve().parent
params = dict(format='json', OBJ_DATA='NO', MAKE_EPHEM='YES', EPHEM_TYPE='VECTORS',
              CENTER='500@399', START_TIME='2026-10-16 00:00:00',
              STOP_TIME='2026-10-18 00:00:00', STEP_SIZE='5 m',
              REF_SYSTEM='ICRF', REF_PLANE='FRAME', VEC_CORR='NONE',
              OUT_UNITS='KM-S', VEC_TABLE='2', CSV_FORMAT='YES', TIME_TYPE='TDB')
records=[]
metadata={'epoch':'2026-10-16 00:00:00 TDB','frame':'geocentric ICRF',
          'provider':'NASA/JPL Horizons','requests':[], 'interval_s':300}
for body in ('10','301'):
    query=params | {'COMMAND':body}
    url='https://ssd.jpl.nasa.gov/api/horizons.api?'+urllib.parse.urlencode(
        {k:(v if k=='format' else "'"+v+"'") for k,v in query.items()})
    with urllib.request.urlopen(url, timeout=60) as response:
        payload=json.load(response)
    if 'error' in payload: raise RuntimeError(payload['error'])
    result=payload['result']
    if '$$SOE' not in result: raise RuntimeError(result)
    (base/f'horizons_{body}.txt').write_text(result)
    table=result.split('$$SOE')[1].split('$$EOE')[0].strip()
    rows=[]
    for row in csv.reader(io.StringIO(table)):
        rows.append([float(row[0])]+[float(x)*1000 for x in row[2:8]])
    records.append(rows)
    metadata['requests'].append({'url':url,'signature':payload.get('signature'), 'target':body})
assert len(records[0])==len(records[1])==577
with (base/'ephemeris.csv').open('w') as f:
    f.write('t_s,sun_x_m,sun_y_m,sun_z_m,sun_vx_mps,sun_vy_mps,sun_vz_mps,moon_x_m,moon_y_m,moon_z_m,moon_vx_mps,moon_vy_mps,moon_vz_mps\n')
    for idx,(sun,moon) in enumerate(zip(*records)):
        assert abs(sun[0]-moon[0])<1e-9
        f.write(','.join(format(x,'.16g') for x in [idx*300]+sun[1:]+moon[1:])+'\n')
metadata['coverage_s']=[0,172800]
(base/'metadata.json').write_text(json.dumps(metadata,indent=2)+'\n')
print('Saved 577 Sun/Moon state samples, 48 hours, TDB.')
