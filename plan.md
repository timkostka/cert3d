# What is Cert3D?

Cert3D is a project designed to certify the performance of 3d printer firmware.  It works by sending test g-code to the printer and monitoring the output of stepper motors.

## Why is Cert3D?

Within the 3d printer firmware community, there is a lack of verification of software features.  A massive amount of man-hours is spent on tracking down bugs which could otherwise be detected automatically.  The aim of this project is to enable developers to ensure features are working as intended before releasing software.

## Who benefits?

For developers, the testing framework helps identify bugs quickly allowing them to be fixed quickly.  The improved software quality due to this benefits.

## How does it work?

There are two parts to Cert3D--A hardware module and a software program.

### The hardware module

The hardware module is hooked up to the 3d printer board and monitors status of the stepper motors and other peripherals.

### The software program

The software sends runs a series of tests by sending g-code to the 3d printer board.  During these tests, it also reads back information from the hardware module.  It analyses these results to ensure the printer is working as intended.
