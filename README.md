## Traffic Light Controller using PIC18F4620

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Setup Instructions](#setup-instruction)
- [Code Explanation](#code-explanation)
    - [Part A) External Interrupts](#part-a-external-interrupts)
    - [Part B) Pedestrian Request Handling](#part-b-pedestrian-request-handling)
    - [Part C) Flashing Mode](#part-c-flashing-mode)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#lincense)

## Introduction

This repository contains the source code and documentation for a traffic light controller implemented using the PIC18F4620 microcontroller. The primary goal of this project is to demonstrate the concept of system interrupts, particularly external interrupts, by simulating a traffic light control system with pedestrian request buttons.

## Features

- Simulates a typical traffic light sequence (Red, Yellow, Green)
- Handles external interrupts to process pedestrian requests
- Configurable timing for each light
- Low power consumption mode
- Debug mode for monitoring the state transitions
- Flashing mode for emergency situations

## Hardware Requirements

- PIC18F4620 Microcontroller
- LEDs (Red, Yellow, Green)
- Resistors for LEDs
- Push buttons (for manual control and pedestrian requests)
- Breadboard and jumper wires
- Power supply (5V)
- Microcontroller programmer (e.g., PICkit 3 or similar)

## Software Requirements

- MPLAB X IDE
- XC8 Compiler

## Setup Instructions

**1. Clone the Repository:**

```bash
git clone https://github.com/derosaseric/traffic-light-controller.git
cd traffic-light-controller
```

**2. Open MPLAB X IDE:**
- Open the **`traffic-light-controller.x`** project file in MPLAB X IDE.

**3. Compile the Code:**
- Ensure the XC8 compiler is selected and build the project.

**4. Program the Microcontroller:**
- Connect your PIC18F4620 to the programmer.
- Use MPLAB IPE or the appropriate software to flash the compiled **`.hex`** file onto the microcontroller.

**5. Assemble the Hardware:**
- Connect the LEDs and resistors to the appropriate ports on the PIC18F4620.
- Connect the push buttons for manual control and pedestrian requests.
- Power up the system.

## Code Explanation

# Part A) External Interrupts

The code initializes and handles external interrupts on the PIC18F4620. External interrupts allow the microcontroller to respond to events such as button presses without constantly checking the input pins.

**- Initialization:**
The **`Do_Init`** function configures the ports and enables interrupts. It sets up interrupt enable bits, interrupt flag bits, and interrupt edge select bits for INT0, INT1, and INT2.

**- Interrupt Service Routines (ISRs):**
The ISRs handle the events triggered by external interrupts. When an interrupt occurs, the corresponding ISR is executed, which clears the interrupt flag and sets a software flag.

**- Main Loop:**
The main loop continuously checks the software flags and processes the interrupts, printing messages to indicate which interrupt has occurred.

# Part B) Pedestrian Request Handling

The project extends the use of external interrupts to manage pedestrian requests in a traffic light system.

**- Pedestrian Request Variables:**
The **`NS_PED_SW`** and **`EW_PED_SW`** variables are used to track pedestrian requests.

**- ISR Modifications:**
The ISRs for INT0 and INT1 are modified to set **`NS_PED_SW`** and **`EW_PED_SW`** when the corresponding button is pressed.

**- Main Traffic Control:**
The main routine from Lab 8 is used, with modifications to handle pedestrian requests using the new variables.

# Part C) Flashing Mode

The project implements a flashing mode for emergency situations, triggered by an external interrupt on INT2.

**- Flashing Request Handling:**
When the INT2 interrupt occurs, it sets a **`FLASHING_REQUEST`** flag. The main program checks this flag and initiates the flashing mode if the flag is set.

**- Flashing Mode Implementation:**
The **`Do_Flashing`** routine manages the flashing of all red lights, exiting when the INT2 button is pressed again.

## Usage

**- Normal Operation:**
The traffic light controller operates normally, cycling through the red, yellow, and green lights.

**- Pedestrian Requests:**
Pressing the push buttons connected to INT0 or INT1 will set pedestrian request flags, which are processed during the normal traffic light sequence.

**- Flashing Mode:**
Pressing the push button connected to INT2 will activate the flashing mode, causing all red lights to flash. Pressing the button again will exit the flashing mode.

## Contributing
Contributions are welcome! Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch (**`git checkout -b feature/your-feature`**).
3. Commit your changes (**`git commit -am 'Add some feature'`**).
4. Push to the branch (**`git push origin feature/your-feature`**).
5. Create a new Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
