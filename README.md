# Interactive Double Pendulum Simulation

This is an interactive simulation of a double pendulum written in C using Raylib. It uses [Runge-Kutta 4](https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods) (RK4) to numerically integrate the differential equations given by the [Euler-Lagrange equation](https://en.wikipedia.org/wiki/Euler%E2%80%93Lagrange_equation). For now, it only works on Windows machines.

## Solving the System

### Setup

![Double Pendulum Diagram](/Drawing.png) 
![Height Diagram](/Drawing2.png) 

The system is comprised of two point mass bodies connected by fixed-length strings. The position of each body is parameterized in terms of the angle from the vertical $\theta_n$. Each body also has a mass $m_n$ and a string length $l_n$. I've defined positive x to be rightward and positive y to be downward to match the rendering.

Based on this, the positions of the two bodies are given by $(x_0, y_0)$ and $(x_1, y_1)$ respectively, where

$$x_0 = l_0\sin\theta_0$$\
$$y_0 = l_0\cos\theta_0$$\
$$x_1 = x_0 + l_1\sin\theta_1$$\
$$y_1 = y_0 + l_1\cos\theta_1$$

### The Lagrangian

To get the differential equations needed to simulate the systems, we need the Lagrangian.

$$ℒ = T - U$$

The kinetic energy

$$T = \frac{1}{2}m_0v_0^2 + \frac{1}{2}m_1v_1^2$$

where

$$v_0^2 = \dot{x_0}^2 + \dot{y_0}^2$$\
$$v_1^2 = (\dot{x_0} + \dot{x_1})^2 + (\dot{y_0} + \dot{y_1})^2$$

And the potential energy

$$U = m_0gh_0 + m1gh_1$$

where

$$h_0 = l_0 - y_0$$\
$$h_1 = h_0 + l_1 - y_1$$

The full Lagrangian, which is quite long, is

$$ℒ = \frac{1}{2}(m_0 + m_1)l_0^2\dot{\theta}_0^2 + \frac{1}{2}m_1l_1^2\dot{\theta}_1^2 + \frac{1}{2}m_1l_0l_1\dot{\theta}_0\dot{\theta}_1\cos(\theta_0 - \theta_1) + (m_0 + m_1)gl_0(1-\cos\theta_0) + m_1gl_1(1-\cos\theta_1)$$

### The differential equations

The differential equations are given by the Euler-Lagrange equation

$$\frac{\partialℒ}{\partial\theta_n} - \frac{d}{dt}\frac{\partialℒ}{\partial\dot{\theta}_n} = 0$$

which will give a system two equations that are each linear in terms of their $\ddot{\theta}_n$ term. Thus, the system of equations can be solved to get

$$\ddot{\theta}_0 = f_0(\theta_0, \dot{\theta}_0, \theta_1, \dot{\theta}_1)$$\
$$\ddot{\theta}_1 = f_1(\theta_0, \dot{\theta}_0, \theta_1, \dot{\theta}_1)$$

This system can be reparameterized to get

$$\omega_0 = \dot{\theta_0}$$\
$$\ddot{\theta}_0 = f_0(\theta_0, \omega_0, \theta_1, \omega_1)$$\
$$\omega_1 = \dot{\theta_1}$$\
$$\ddot{\theta}_1 = f_1(\theta_0, \omega_0, \theta_1, \omega_1)$$

These equations are now in a form suitable for numerical integration via Runge-Kutta 4 or any other numerical integration technique. I've chosen RK4 because it's relatively easy to implement and performs quite well.

## Simulation

![Simulation](/sim.png) 

The simulation can be ran with `Double_Pendulum.exe`, which creates a fixed-size 1920x1080p window. You can exit the window by pressing escape. In the future, I might make the window resizable, but that would require overhauling the rendering and UI to make it responsive.

I've written the simulation in C using Raylib for getting input and rendering. You can start and stop the simulation by pressing the spacebar or clicking the Start/Stop button. You can also increase or decrease the speed using the left and right arrow keys. To change the initial configuration, you can use the sliders. Finally, I've included the initial and final energy as well as the percent change between the two. The energy values don't really correspond to real world values but are somewhat interesting nonetheless.

I've also included a single pendulum, though that one is more primitive. I started on an N-Body simulation, but it's very much incomplete.

The compilation is done in the command line via `make.bat [sim]`, where `sim` is either `single`, `double`, `body`, or `all`. This will create an executable in the `build` folder, which can be ran via `run.bat [sim]`. Be warned that `make.bat` requires Visual Studio 2022 to be in the default `C:` directory and will not work otherwise. In the future, I might consider creating a CMake file to universalize the build process.

## N-Body Simulation

I initially wanted to simulate a system with N bodies, but that turned out to be a significantly more complex problem than I had anticipated. To support N bodies, I'd need to express everything as some sort of sum, and I'd also need to solve a variable system of linear equations. I found a [paper](https://arxiv.org/abs/1910.12610) that would prove useful for the first part, but I'd either need to solve the linear system by hand or incorporate some sort of scientific computation library to solve it for me. 
