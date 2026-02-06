# OrbitalAnimation
# Orbital Physics Visualizer (C++ | SFML)

High-performance real-time orbital simulation and visualization engine built in C++ using SFML.  
Designed to demonstrate orbital mechanics, perturbation modeling concepts, and interactive simulation rendering.


## Project Overview

This project simulates satellite motion around Earth using Newtonian gravity with additional perturbation-inspired effects and real-time visualization.

It combines:
- Physics simulation
- Real-time rendering
- Interactive camera control
- Multi-satellite system
- Orbit prediction visualization

## Key Features

### 🛰 Multi-Satellite Simulation
- Multiple satellites orbit Earth simultaneously
- Each satellite maintains independent velocity and state

### Orbit Trail Rendering
- Real-time orbit path visualization
- Long-duration trail persistence

### Physics-Based Orbital Motion
- Newtonian gravity simulation
- Energy monitoring (KE + PE)
- Stable orbit velocity initialization

###  Perturbation-Inspired Drift
- J2-style perturbation inspired drift simulation
- Demonstrates non-perfect Keplerian orbit behavior

###  Orbit Prediction Path
- Forward integration ghost orbit path visualization
- Useful for trajectory planning concepts

### Interactive Controls
| Control | Action |
|---|---|
| Mouse Wheel | Zoom |
| WASD | Camera Pan |
| Left Click | Spawn New Satellite |


## 🛠 Tech Stack

- **Language:** C++
- **Graphics:** SFML 3
- **Physics:** Custom Newtonian Simulation
- **Architecture:** Real-time game-loop style simulation engine

## Physics Concepts Demonstrated

- Two-body gravitational motion
- Orbital velocity initialization
- Energy conservation monitoring
- Perturbation modeling concept (J2 inspired)
- Numerical forward trajectory prediction

## Example Simulation Capabilities

- Real-time orbital rendering
- Dynamic satellite spawning
- Camera navigation in simulation space
- Predictive orbit visualization

## Future Extensions

- RK4 orbital propagator integration
- Real J2 perturbation equation implementation
- 3D OpenGL orbital visualization
- TLE orbit import and visualization
- Mission planning interface overlay


##  Author

Yash Anand  
BSc Physics | Orbital Mechanics Simulation | Space Software Engineering Path


## Purpose

Built as part of a long-term goal toward high-performance aerospace simulation and spaceflight dynamics software engineering.
