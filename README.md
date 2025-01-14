# Interactive Double Pendulum Simulation

This is an interactive simulation of a double pendulum written in C using Raylib. It uses [Runge-Kutta 4](https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods) (RK4) to numerically integrate the differential equations given by the [Euler-Lagrange equation](https://en.wikipedia.org/wiki/Euler%E2%80%93Lagrange_equation).

## Solving the System

### Setup

![Double Pendulum Diagram](/Drawing.png) 
![Height Diagram](/Drawing2.png) 

The position of each body is parameterized in terms of the angle from the vertical $\theta_n$. Each body also has a mass $m_n$ and a string length $l_n$. I've defined positive x to be rightward and positive y to be downward to match the rendering.

The potential energy $U = m_0gh_0 + m_1gh_1$, where $h_n$ is the difference between the awefe

This is an equation $\int_{-\infty}^{\infty}e^{-x^2}dx = \sqrt{\pi}$
[N-Body Equations Paper](https://arxiv.org/abs/1910.12610) 
