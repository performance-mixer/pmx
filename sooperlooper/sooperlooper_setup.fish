#!/usr/bin/fish

# Global settings - valid for all loops
oscsend osc.udp://127.0.0.1:9951 /set sf sync_source -2
oscsend osc.udp://127.0.0.1:9951 /set sf eighth_per_cycle 64

# Loop settings
for i in (seq 0 7)
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf sync 1
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf playback_sync 1
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf quantize 1
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf use_common_ins 0
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf use_common_outs 0
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf dry 1
    oscsend osc.udp://127.0.0.1:9951 /sl/$i/set sf wet 0
end
