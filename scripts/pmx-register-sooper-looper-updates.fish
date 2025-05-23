#!/usr/bin/fish
oscsend osc.udp://127.0.0.1:9951 /sl/0/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/1/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/2/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/3/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/4/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/5/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/6/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/7/register_auto_update siss state 100 osc.udp://127.0.0.1:12345 /sl/loops

#!/usr/bin/fish
oscsend osc.udp://127.0.0.1:9951 /sl/0/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/1/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/2/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/3/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/4/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/5/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/6/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops
oscsend osc.udp://127.0.0.1:9951 /sl/7/register_update sss state osc.udp://127.0.0.1:12345 /sl/loops

oscsend osc.udp://127.0.0.1:9951 /register_auto_update siss eighth_per_cycle 100 osc.udp://127.0.0.1:12345 /sl/global
oscsend osc.udp://127.0.0.1:9951 /register_update sss eighth_per_cycle osc.udp://127.0.0.1:12345 /sl/global
