# Batt-LP-RF433-18dsb20
Battery powered Arduino nano featuring 433 MHz TX with ds18b20 temperature sensor

This repo is based on the [rfbox-ds18b20.ino](https://github.com/incmve/generic-rfbox/blob/development/Sketches/rfbox-ds18b20.ino) from the generic rf-box by incmve: https://github.com/incmve/generic-rfbox<br>
Further sources (of information) used:
- http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
- https://code.google.com/p/tinkerit/wiki/SecretVoltmeter

What has been added:
- The Rocket Stream Low power library: https://github.com/rocketscream/Low-Power
- Battery measurement

This repo consists of two sketches:
- One without battery measurement: [rfbox-ds18b20-LP.ino](rfbox-ds18b20-LP.ino)
- One with battery measurement: [rfbox-ds18b20-LP-Voltage.ino](rfbox-ds18b20-LP-Voltage.ino)

Please note that this is definitely not a "state of the art" sketch but it works and can be a possible source of information for you.

To reduce further power draining components I wanted to remove the power led. However when trying to remove it with plyers I broke the led but it had the same effect. The Arduino nano still worked and the power use was reduced.

Schema below:<br>
(Everything is added to a breadboard but that made the schema less readable)<br>
![schema](RF-ds18b20-LP-NoBreadboard.jpg)

