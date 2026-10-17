#!/bin/bash
set -euo pipefail
source "$(dirname "$0")/environment.sh"
scenario="${1:-nominal}"
case "$scenario" in nominal|low_power) ;; *) echo 'Kullanım: tools/run.sh [nominal|low_power]' >&2; exit 2;; esac
mkdir -p "$LAB_ROOT/artifacts"
cd "$LAB_ROOT/sim/SIM_satellite"
trick-CP > "$LAB_ROOT/artifacts/build.log" 2>&1
shopt -s nullglob
executables=(./S_main*.exe)
if [[ ${#executables[@]} -ne 1 ]]; then echo 'Tek bir simülasyon yürütülebilir dosyası bekleniyor.' >&2; exit 1; fi
"${executables[0]}" "RUN_$scenario/input.py" > "$LAB_ROOT/artifacts/$scenario.log" 2>&1
python3 "$LAB_ROOT/tools/export_demo.py" "$scenario"
echo "Görüntüleyici: $LAB_ROOT/web/index.html"
echo 'Tarayıcıda web/index.html dosyasını açın veya tools/serve.py kullanın.'
