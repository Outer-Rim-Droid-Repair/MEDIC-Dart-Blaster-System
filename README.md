MEDIC Dart Blaster System
==============

The MEDIC (Multiple Electronic Distributed Interconnected Controllers) Dart Blaster System is an attempt to to make a modular system to add/expand functions to any dart balster.
This projects started as an upgrade to the XShot Motorized Piston Pumps control board but the scope ofthatproject grew past those bounds and into the MEDIC system.

__This project is very much a work in progress and features mentioned may not be implemeted yet__

Project Thesis
-----------

The MEDIC system is a collection of standalone modules that handle a unique aspect of a dart blaster. Each can be used by themselves or can be used in any combination with other modules without interfering. When a controll modules is included in the network that control module allows the user to modify the settings of other modules in the network. The controller also adds a screen that can access the states of all modules.

Planned Features
-----------

- Power Distribution Board: Allow for any LiPo from 2S up to 6S and can requlate the voltage down any lower cell count as well as providing logic voltage for all boards. Also includes cell voltage monitoring and alarm capabilities.
-  Chronograph: A muzzle attaching device to act as a chrono/DPS tracker/shot counter. Can be run in 1 beam mode to sacrifice the chrono ability but gaining a  smaller size.
-   Fire Control Board: Handle the firing process. Varrious methods of controlbased on the type of blaster.
  -   2 Switch AEB. Like the XShot Motorized Piston Pump. in these there are 2 limit switches. One in the breach for when it is closed and another at theback of the plunger travel.
  -   0 Switch AEB. Like the Nerf Vulcan. These run the motor as long as the trigger is held down and can't identify where is the firing process it is.
  -   Flywheelers. Looking to support brushed up to 2 stagewith on board motor drivers. May expand to brushless once I look into brushless motor drivers.
  -   Mixed Methods. Like a Nerf Vulcan with an afterburner. A unit that uses multiple formsof propulsion. 

Connumication Method
------------

All modules are connected with 4 wires 3.3V Logic voltage, ground, I2C Data, and I2C Clock. These allow for controller modules to talk to other modules through the use of the MEDIC Comms Library. More information will come as further work happens on the  library.
