# Batt-LP-RF433-18dsb20
Battery powered Arduino nano featuring 433 MHz TX with ds18b20 temperature sensor

This repo is based on the generic rf-box by incmve: https://github.com/incmve/generic-rfbox

What has been added:
- The Rocket Stream Low power library: https://github.com/rocketscream/Low-Power
- Battery measurement

This repo consists of two sketches:
- One without battery measurement: [rfbox-ds18b20-LP.ino](rfbox-ds18b20-LP.ino)
- One with battery measurement: [rfbox-ds18b20-LP-Voltage.ino](rfbox-ds18b20-LP-Voltage.ino)

Schema below:
![schema](RF-ds18b20-LP-NoBreadboard.jpg)

