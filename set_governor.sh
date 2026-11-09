#!/bin/sh
# CPU clock policy for all cores (needs sudo).  sh set_governor.sh performance | schedutil
#   performance: clock stays at maximum -> mel 48 -> 17 ms, encoder stage 86 -> 71 ms, decoder step 18.8 -> 17.7 ms (board, FLEURS 70)
#   schedutil:   board default (clock follows load; drops while the CPU waits for the NPU)
# Resets at reboot. To keep it after reboot:  sh set_governor.sh performance boot
G=${1:-performance}
sudo sh -c "for p in /sys/devices/system/cpu/cpufreq/policy*; do echo $G > \$p/scaling_governor; done"
echo "governor: $(cat /sys/devices/system/cpu/cpufreq/policy*/scaling_governor | sort -u | tr '\n' ' ')"
if [ "$2" = boot ]; then
  printf '[Unit]\nDescription=CPU governor for Whisper ASR\n[Service]\nType=oneshot\nExecStart=/bin/sh -c "for p in /sys/devices/system/cpu/cpufreq/policy*; do echo %s > $$p/scaling_governor; done"\n[Install]\nWantedBy=multi-user.target\n' "$G" |
    sudo tee /etc/systemd/system/whisper-governor.service > /dev/null
  sudo systemctl daemon-reload && sudo systemctl enable whisper-governor.service && echo "kept after reboot (undo: sudo systemctl disable whisper-governor.service)"
fi
