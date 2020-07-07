# Synchrono Interface Module
Tested Systems:
 - Arch Linux: 10/28/2019

## Description
 - Allows communication between Chrono and an external controller

## Dependencies
 - Chrono (required)
	 - Repo: uwsbel/chrono-dev \
   - Branch: sandbox/sensor \
   - Commit: 7c44feea5bee9dd068245f23470bd1409a4626d6 \
   - Enabled modules: vehicle, irrlicht, sensors
 - (FlatBuffers)[https://google.github.io/flatbuffers/] (required)
   - tested with version 1.11.0

## CMake and Build Notes
 - Must build Chrono with above modules enabled.
 - Must enable Interface module

## Getting started with the demos
 - demo_interface
	 - builds if Synchrono_Interface and Build_Demos are enabled
	 - Creates vehicle with variety of sensors. Creates Interface and waits for receiver.

## Current Capabilities
 - Outgoing message
 - Generic SynInterfaceComponent
   - Must implement custom component for flatbuffers message to be sent

## Capabilities in Progress
 - FlatBuffer .fbs generator.
