# Source from the supplied run/check scripts; no machine-specific home directory.
LAB_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ -f "$LAB_ROOT/config.local.sh" ]]; then source "$LAB_ROOT/config.local.sh"; fi
if [[ -z "${TRICK_HOME:-}" && -d "$LAB_ROOT/../trick/bin" ]]; then
    export TRICK_HOME="$(cd "$LAB_ROOT/../trick" && pwd)"
fi
if [[ -n "${TRICK_HOME:-}" ]]; then export PATH="$TRICK_HOME/bin:$PATH"; fi
if [[ "$(uname -s)" == Darwin && -d /opt/homebrew ]]; then
    export JAVA_HOME="${JAVA_HOME:-/opt/homebrew/opt/openjdk/libexec/openjdk.jdk/Contents/Home}"
    export PATH="/opt/homebrew/opt/bison/bin:$JAVA_HOME/bin:/opt/homebrew/opt/python@3.12/bin:/opt/homebrew/bin:$PATH"
    export PYTHON_VERSION="${PYTHON_VERSION:-3.12}"
    export TRICK_CFLAGS="${TRICK_CFLAGS:--g -I/opt/homebrew/include -Wno-unused-command-line-argument}"
    export TRICK_CXXFLAGS="${TRICK_CXXFLAGS:--g -std=c++17 -I/opt/homebrew/include -Wno-unused-command-line-argument}"
    export TRICK_LDFLAGS="${TRICK_LDFLAGS:--L/opt/homebrew/lib}"
    export TRICK_EXCLUDE="${TRICK_EXCLUDE:-/opt/homebrew}"
    export LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
fi
if ! command -v trick-CP >/dev/null; then
    echo 'Trick bulunamadı. TRICK_HOME değerini config.local.sh içinde veya terminalde ayarlayın.' >&2
    return 1
fi
