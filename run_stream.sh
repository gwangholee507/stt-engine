#!/bin/sh
# Run board_stream.py with the QNN 2.50 bundle libraries.  e.g.  sh run_stream.sh --fleurs 15
B=${B:-/home/ubuntu/rubikpi/sdk250}
export LD_LIBRARY_PATH=$B/gcc11.2/lib ADSP_LIBRARY_PATH="$B/dsp;/usr/lib/rfsa/adsp;/dsp"
cd "$(dirname "$0")"
# Fast cores only (highest max clock; PIN=0 to turn off): CPU-side work is ~2x slower on the small cores.
BIG=$(for c in /sys/devices/system/cpu/cpu[0-9]*; do echo "$(cat $c/cpufreq/cpuinfo_max_freq 2>/dev/null || echo 0) ${c##*cpu}"; done |
      sort -n | awk '{f[NR]=$1;id[NR]=$2} END{for(i=1;i<=NR;i++) if(f[i]>f[1]) s=s (s?",":"") id[i]; print s}')
[ "$PIN" = 0 ] && BIG=; [ -n "$BIG" ] || export WHISPER_PIN=0
${BIG:+taskset -c $BIG} python3 board_stream.py "$@"
