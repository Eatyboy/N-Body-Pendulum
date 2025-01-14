# Interactive Double Pendulum Simulation

This is an interactive simulation of a double pendulum written in C using Raylib. It uses [Runge-Kutta 4](https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods) (RK4) to numerically integrate the differential equations given by the [Euler-Lagrange equation](https://en.wikipedia.org/wiki/Euler%E2%80%93Lagrange_equation).

## Solving the System

### Setup

![Double Pendulum Diagram](/Drawing.png) 
![Height Diagram](/Drawing2.png) 

The position of each body is parameterized in terms of the angle from the vertical $\theta_n$. Each body also has a mass $m_n$ and a string length $l_n$. I've defined positive x to be rightward and positive y to be downward to match the rendering.

The potential energy is the gravitational potential energy $mgh$, where $h$ is the difference between a body's y-position and the y-position where the potential is at the minimum.

### The Lagrangian

$\mathcal{L}$


[N-Body Equations Paper](https://arxiv.org/abs/1910.12610) 
