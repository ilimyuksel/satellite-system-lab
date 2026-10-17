#!/bin/bash
set -euo pipefail
source "$(dirname "$0")/environment.sh"
mkdir -p "$LAB_ROOT/artifacts"
cd "$LAB_ROOT/sim/SIM_satellite"
python3 "$LAB_ROOT/tools/prepare_build.py"
trick-CP > "$LAB_ROOT/artifacts/build.log" 2>&1
c++ -std=c++17 -Imodels "$LAB_ROOT/tests/environment_test.cpp" -o "$LAB_ROOT/artifacts/environment_test"
"$LAB_ROOT/artifacts/environment_test"
c++ -std=c++17 -Imodels "$LAB_ROOT/tests/power_test.cpp" models/power/src/Power.cpp models/payload/src/Payload.cpp models/system/src/Spacecraft.cpp models/thermal/src/Thermal.cpp -o "$LAB_ROOT/artifacts/power_test"
"$LAB_ROOT/artifacts/power_test"
c++ -std=c++17 -Imodels "$LAB_ROOT/tests/thermal_test.cpp" models/power/src/Power.cpp models/payload/src/Payload.cpp models/system/src/Spacecraft.cpp models/thermal/src/Thermal.cpp -o "$LAB_ROOT/artifacts/thermal_test"
"$LAB_ROOT/artifacts/thermal_test"
shopt -s nullglob
executables=(./S_main*.exe)
[[ ${#executables[@]} -eq 1 ]]
for scenario in nominal low_power cold cold_no_heater validation j2 drag halfstep; do
    "${executables[0]}" "RUN_$scenario/input.py" > "$LAB_ROOT/artifacts/$scenario.log" 2>&1
done
python3 "$LAB_ROOT/tests/validate_system.py"
python3 "$LAB_ROOT/tests/validate_thermal.py"
python3 "$LAB_ROOT/tests/validate_orbit.py"
python3 "$LAB_ROOT/tools/export_demo.py" nominal
