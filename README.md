# 6-DOF Modular Robotic Arm: Bare-Metal Control

A 6-axis robotic arm prototyped in cardboard, designed in Siemens NX/Fusion 360, laser-cut from plywood and acrylic, and controlled via register-level programming on the ATmega328P (Arduino Uno).

## Mechanical Design & Iteration
The physical structure was developed through a multi-stage prototyping process:

1. **Low-Fidelity Prototyping:** Constructed an initial model using cardboard sheets to validate the 6-Degrees of Freedom (DOF) mechanics and joint ranges.
2. **CAD Modeling (Siemens NX):** Developed a 3D assembly in Siemens NX to define the structural layout and pivot points.
3. **Refinement & Export (Autodesk Fusion):** Transitioned the design to Fusion 360 to refine individual components and export precise 2D sketches for manufacturing.
4. **Manufacturing & Assembly:** * **Chassis:** Laser-cut from 3mm Plywood for an optimal strength-to-weight ratio.
    * **Claw/Gripper:** Laser-cut from 3mm Acrylic. Acrylic was chosen over plywood for the end-effector to ensure smoother gear engagement and higher precision in the gripping mechanism.
    * **Modular Design:** The arm is built with a modular philosophy, allowing individual segments or servos to be swapped out for maintenance or upgrades.

## Hardware Stack
* **Microcontroller:** Arduino Uno (ATmega328P)
* **Actuators:** 6x SG90 Micro Servos
* **Power Supply:** WSP3010H Bench Power Supply
* **Materials:** 3mm Plywood, 3mm Acrylic, M3 screws and nuts

## Firmware & Electronics
The control system is built using a Bare-Metal C++ approach, interacting directly with the ATmega328P hardware registers.]

## Control Goals
 **6 Working Servos with preprogrammed sequence**
 **Inverse Kinematics implemntation to reach pre-programmed positions**

##  Repository Structure
* `/src`: Contains the `.cpp` and `.h` files featuring register-level setup and ISR logic.
* `/cad`: Includes exported DXF files for laser cutting and 3D assembly references.
* `/docs`: Wiring schematics and register mapping tables for the ATmega328P.

## Roadmap
* **Iteration 2:** Re-design to be 3D printed and with higher torque MG996R servos. Integration of a potentiometer twin that can be used to control the arm and set a path that can be repeated. Potential implementation of computer vision.
* **Iteration 3:** 3D printed design using stepper motors rather than servos to improve precison and smooth movement.

## Preview
<img width="1259" height="992" alt="Screenshot 2026-04-07 222445" src="https://github.com/user-attachments/assets/1f871e1f-59cf-4a8e-828b-3e082033ea50" />
<img width="3060" height="1831" alt="Screenshot 2026-04-07 222706" src="https://github.com/user-attachments/assets/825e9896-372e-4f9e-bd72-f2fb675c3733" />
<img width="3058" height="1825" alt="image" src="https://github.com/user-attachments/assets/6f0936e8-013d-4845-b46d-0486217360af" />

