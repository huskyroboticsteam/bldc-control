# BLDC Board

## BLDC Motor Overview

A brushless DC (BLDC) motor consists of a permanent magnet in the center that rotates and coils that surround the permanent magnet. In order
to rotate the motor, current is passed through the coils, which generates a magenetic field. Changing the direction of these magnetic fields
cause the permanent magnet to rotate. The motor we are using is a 3 phase motor, which means there are 3 sets of coils. The motor also has 
hall sensors which tell us the orientation of the permanent magent. In order to drive the motor, we need to continuously take readings from 
the hall sensors, then decide which phases to sent current through and the direction of magnetic field to generate. There are 6 different 
phase configurations you need to cycle through. For more detailed information, check out our presentation on bldc motors! 

### Additional Resources:
Research: https://docs.google.com/document/d/1kpRwuTik46sPaxE6ltFqlqkKr64m_XjKgY7ZmlLvnd8/edit?usp=sharing
Presentation: https://docs.google.com/presentation/d/1GThbB09xTLLST71g931fdWaHDSGAaZzSrz8cfOe4D8c/edit?usp=sharing

## Driving the BLDC Motor

The board consists of a integrated gate driver and 3 half-bridges, which each contain 2 MOSFETS. Each phase is connected to two MOSFETs 
(one high and one low) and to set a certain phase configuration we will open or close specific MOSFETs. Each half-bridge has a constant
voltage and the PWM signal we send in to the gate driver will control how quickly the MOSFET opens and closes, which essentially sends a 
new matching PWM signal to the motor. For each PWM signal we send to a high MOSFET, we send an inverse PWM signal to the corresponding low
MOSFET and delay it slightly (deadtime) so they aren't open at the same time. This prevents shootthrough, which is when current goes 
straight through MOSFETs instead of through motor. We are using 6x mode which means we are individually driving each MOSFET since we think 
its cool, but the driver supports other modes such as 1x that just takes in hall sensor readings and single PWM signal and will figure it 
all out for you!

## Hardware

Hall sensor: https://www.mouser.com/datasheet/2/588/AS5048_DS000298_4-00-1100510.pdf
Half-Bridge MOSFETs: https://www.ti.com/lit/ds/symlink/csd88599q5dc.pdf
Gate Driver: https://www.ti.com/lit/ds/symlink/drv8323.pdf
odrive spec: https://www.notion.so/Projects-284633f5896d80e69232da7a280690be?p=284633f5896d80149b6ffc7ecf2cd94d&pm=s
  - we are making a driver to replace odrive

## Repository Layout

main: 
phases: Generates PWM signal using hall sensor readings
hall sensor: Takes hall sensor readings and converts it to an `uint8_t`. Also calculates motor speed and direction. 
SPI driver:
PID: Calculates PID. Uses speed as input and PWM as output. 
CAN: 
