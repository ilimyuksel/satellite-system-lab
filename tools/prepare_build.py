#!/usr/bin/env python3
"""Invalidate Trick's generated parent IO when nested C++ layouts change."""
import hashlib
from pathlib import Path
root=Path(__file__).resolve().parents[1]
sim=root/'sim/SIM_satellite'
headers=sorted((sim/'models').rglob('*.hh'))
digest=hashlib.sha256(b''.join(str(p.relative_to(sim)).encode()+p.read_bytes() for p in headers)).hexdigest()
marker=root/'artifacts/model-headers.sha256'
if not marker.exists() or marker.read_text()!=digest:
    for p in headers+[sim/'S_define']:p.touch()
    marker.parent.mkdir(exist_ok=True)
    marker.write_text(digest)
    print('Model headers changed: regenerating Trick bindings.')
