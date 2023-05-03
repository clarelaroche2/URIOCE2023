# URIOCE2023
2023 URI Acoustics Capstone Group
The following repository was designed for the URI Ocean Engineering capstone group tasked with designing a low-cost hydrophone for passive acoustics. The repository includes the code to run a teensy microcontroller at a sampling frequency of 44.1 kHz to gather acoustic data, code for the user to set the hydrophone settings before each mission, and code to assist with processing the data after the mission. 

The following folders contain C++ code which should be downloaded directly onto the teensy microcontroller via USB: Teensy RTC Set, Teensy Sampling with Serial, Teensy Sampling Final

HydrophoneSettings.py allows the user to change the hydrophone settings. The user should upload this file to the SD card. 

DataProcessingLib.py contains functions to be used for post processing. 
 
