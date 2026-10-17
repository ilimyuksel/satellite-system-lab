"""Read Trick CSV by variable names, independent of column order."""
import csv
from pathlib import Path

def read(path):
    with Path(path).open() as f:
        reader=csv.reader(f)
        names=[x.split(' {')[0].replace('dyn.satellite.','').replace('sys.exec.out.time','time') for x in next(reader)]
        rows=[dict(zip(names,map(float,row))) for row in reader if row]
    if len(rows)<2: raise ValueError('At least two records required')
    return rows
