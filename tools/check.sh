#!/bin/bash
set -euo pipefail
source "$(dirname "$0")/environment.sh"
mkdir -p "$LAB_ROOT/artifacts"
cd "$LAB_ROOT/sim/SIM_satellite"
trick-CP > "$LAB_ROOT/artifacts/build.log" 2>&1
c++ -std=c++17 -Imodels "$LAB_ROOT/tests/environment_test.cpp" -o "$LAB_ROOT/artifacts/environment_test"
"$LAB_ROOT/artifacts/environment_test"
c++ -std=c++17 -Imodels "$LAB_ROOT/tests/power_test.cpp" models/power/src/Power.cpp models/payload/src/Payload.cpp models/system/src/Spacecraft.cpp -o "$LAB_ROOT/artifacts/power_test"
"$LAB_ROOT/artifacts/power_test"
shopt -s nullglob
executables=(./S_main*.exe)
[[ ${#executables[@]} -eq 1 ]]
for scenario in nominal low_power validation j2 drag halfstep; do
    "${executables[0]}" "RUN_$scenario/input.py" > "$LAB_ROOT/artifacts/$scenario.log" 2>&1
done
python3 "$LAB_ROOT/tests/validate_system.py"
python3 "$LAB_ROOT/tests/validate_orbit.py"
python3 "$LAB_ROOT/tools/export_demo.py" nominal
