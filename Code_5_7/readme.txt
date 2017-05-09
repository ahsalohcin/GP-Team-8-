GrandPrIEEECode: the code used for Grand PrIEEE competition that got 36 sec and 3rd place.

vRef: 9.5 ft/sec. adding phone camera with same vRef got a 33 to 34 sec runtime. 

Batt voltage: ~ 8.1 V
Camera setting: 5th notch down
Looptime: 
Line detection: median filter using 5 elements. 
Line widths: 1 - 15 pixels.
Slightly faulty line loss handling. Always goes left. Not using time averaging. 

Steering Kp: 9.84 
Speed Kp: 3.0 Ki: .05
Slow down on turns: speed drop is proportional to curvature, with a slope of 2.5, and maximum drop of 2 ft/sec

Operation:

Turn on Teensy switch. Turn on transmitter. Right joystick max up. Turn on big switch. Expect slight rev, then pause. Release right joystick to start. Adjust vRef using Dip switches