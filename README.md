# X-Shot Motorized Piston Pump Fire Control Board
This project is an excuse to play around with electronics in a way that is different from my day job. This code is being developed on a feather 32u4 but may be moved once more feature complete and in the miniaturization process.

Does this code function better than the board that the MPP came with? No, at least not yet. I hope to have at least burst fire working in short order to make this project different than what you get out of the box.

# Features
* Fire Modes
  - Single Fire
  - Burst fire
  - Full Auto
  - Binary (planned)
  - Ramping Fire (planned)
* Adjustable Max Fire Rate (partial implementation)
* Adjustable Burst Size
* Variable idle positions
* SW safety (on top of mechanical safety)
* Voltage Monaturing (planned)

# Pinout
<img width="556" height="306" alt="image" src="https://github.com/user-attachments/assets/16f2b187-d789-4277-9f6d-4b2033a0c4c7" />

# Project Pictures
Coming soon

# Potential expansions on the project
While the base platform of the MPP is solid there are some limitations in being a low cost AEB. At a point it no longer makes sense to keep putting effort in when one of the more expensive platforms would serve me better. But that's not the point of this project. Below are some ideas I have for improvements to the platform that I will explore as this project matures and hobby funds open up.

1. Larger battery. Probably the easiest way to get a higher fire rate and snappier response times is more voltage. Currently waiting for parts. The main concern I have with this is the unknown motor as I can not see any manufacturing numbers on it that lead to any sort of specs.
2. Upgraded Motor. On top of improving the quality of the motor and upgraded motor would have a known datasheet. It may also increase fire rate depending on the motor selects. As far as I can tell the original motor in the MPP is the same form factor as an airsoft AEG short shaft motor.
3. Improved Plunger. The internals look to share most of their geometry with the BK1/2S and that platform has a handful of upgraded plungers to improve the seal when firing.Low priority
