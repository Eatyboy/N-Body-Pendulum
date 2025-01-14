# Interactive Double Pendulum Simulation

This is an interactive simulation of a double pendulum written in C using Raylib. It uses [Runge-Kutta 4](https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods) (RK4) to numerically integrate the differential equations given by the [Euler-Lagrange equation](https://en.wikipedia.org/wiki/Euler%E2%80%93Lagrange_equation).

## Solving the System

### Setup

![Double Pendulum Diagram](/Drawing.png) 
![Height Diagram](/Drawing2.png) 

The position of each body is parameterized in terms of the angle from the vertical $\theta_n$. Each body also has a mass $m_n$ and a string length $l_n$. I've defined positive x to be rightward and positive y to be downward to match the rendering.

Based on this, the positions of the two bodies are given by $(x_0, y_0)$ and $(x_1, y_1)$ respectively, where
$$x_0 = l_0\sin\theta_0$$
$$y_0 = l_0\cos\theta_0$$
$$x_1 = x_0 + l_1\sin\theta_1$$
$$y_1 = y_0 + l_1\cos\theta_1$$

### The Lagrangian

To get the differential equations needed to simulate the systems, we need the Lagrangian.
$$ℒ = T - U$$

The kinetic energy 
$$T = \frac{1}{2}m_0v_0^2 + \frac{1}{2}m_1v_1^2$$
where 
$$v_0^2 = \dot{x_0}^2 + \dot{y_0}^2$$ 
$$v_1^2 = (\dot{x_0} + \dot{x_1})^2 + (\dot{y_0} + \dot{y_1})^2$$

And the potential energy
$$U = m_0gh_0 + m1gh_1$$
where 
$$h_0 = l_0 - y_0$$ 
$$h_1 = h_0 + l_1 - y_1$$

The full Lagrangian, which is quite long, is
$$ℒ = \frac{1}{2}(m_0 + m_1)l_0^2\dot{\theta}_0^2 + \frac{1}{2}m_1l_0l_1\dot{\theta}_0\dot{\theta}_1\cos(\theta_0 - \theta_1)$$

[N-Body Equations Paper](https://arxiv.org/abs/1910.12610) 
