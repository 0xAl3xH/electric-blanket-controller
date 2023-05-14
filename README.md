# Electric Blanket Controller 

## About The Project

An excellent way to stay warm while camping/overlanding on a tight electrical energy budget is with a 12V electric blanket. With a blanket such as [this one](https://www.amazon.com/gp/product/B01N21MI0Z) under a sleeping bag, you can stay fairly warm. However, these blankets don't typically come with temperature control and you can easily overheat a couple hours after power on. In addition to it being uncomfortable, there's power being wasted here to achieve a comfortable temperature. The electric blanket controller uses a MOSFET as a switch to apply a duty cycle to the power coming into the blanket, giving users a choice between different levels of heat. 

## Goals

- [ ] Efficient - as most users are using 12V electric blankets off grid, it's important that the controller doesn't dissipate much power
- [ ] Easy to use - no modifications to the blankets or car power port, plug and play
- [ ] Replicable - so more overlanders can stop overheating with these blankets and save energy

## Milestones

- [ x ] Find appropriate hardware for controller
- [ x ] Test hardware and find duty cycle
- [ x ] Create PCB 
- [ x ] Write firmware
- [ x ] Test firmware
- [ x ] Build controller on real PCB with v1.0.0 firmware
- [ x ] Design a 3D printed case

## Getting Started

Flash the `.ino` file using your favorite ATTiny programmer. I didn't have one handy so ended up using an [Arduino Uno](https://www.instructables.com/How-to-Program-an-Attiny85-From-an-Arduino-Uno) to flash the ATTiny. The ATTiny needs to run at 1 MHz clock for this project, which should be the default from factory. The `hardware` directory contains the circuit board schematic and board files in `pcb` as well as a 3D printed case with tight and loose tolerances to account for the variablility in 3D printers. 

## License

Distributed under the MIT License. See LICENSE.txt for more information.

