# Code

This folder contains all of the example code used throughout the course. Each subfolder represents a self-contained project that can be built and flashed to the Daisy Seed.

> **Important**
>
> These project folders are intended to live inside the official `DaisyExamples` directory.
>
> By default, the Makefiles assume the following structure:
>
> - `libDaisy` is located at `../../libDaisy`
> - `DaisySP` is located at `../../DaisySP`
>
> If you place these projects somewhere else, you will need to update the paths in the Makefile accordingly.

---

## Project Folders

- [ChorusPedal](./ChorusPedal)  
  The main chorus pedal project built during the course. This is final version configured for the PCB found [here](../hardware/chorus_pedal/).

- [ChorusPedalPod](./ChorusPedalPod)  
  Version of the chorus pedal that I wrote during the course, written for the Daisy Pod (not configured yet for the PCB).

- [DistortionPedalPod](./DistortionPedalPod)  
  The soft/hard clipping distortion pedal made during Pt. 2 of the course. This version is configured for the pod, where you can press a button to switch between the hard and soft clipping distortion.

- [ChorusPedalArduinoIDE](./ChorusPedalArduinoIDE)  
  A version of the chorus pedal adapted for use with the Arduino IDE instead of the standard Makefile workflow. Included for those curious.

Each folder includes its own source files and build configuration.
