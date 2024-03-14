import casadi as cs 
import numpy as np
import fatropy.spectool as sp
import double_inverted_pendulum_dynamics

# control grid parameters
N = 50 # number of time steps
T = 2.0 # time horizon

# Instantiate an Ocp 
ocp = sp.Ocp()

# add a stage to the ocp (N = 50 time steps)
stage = ocp.new_stage(N)

# define the state and control variables
theta_pole = ocp.state(2) # angle of the pole, measured from the vertical
dtheta_pole = ocp.state(2) # angular velocity of the pole
F = ocp.control() # horizontal force applied to the cart
x = [theta_pole, dtheta_pole]
ux = [F] + x

# create your problem parameters -> value at time step 0 for each state
theta_pole_0 = ocp.parameter(2)
dtheta_pole_0 = ocp.parameter(2)

# Define the system dynamics
x_dot, mechanism, _, _ = double_inverted_pendulum_dynamics.ode(*x, F)

# take a runge kutta 4 integrator
intg = sp.IntegratorRk4(list(zip(x, x_dot)), T/N)
for x, xn, in zip(x, intg(x)):
    stage.set_next(x, xn) 

# Define the cost function
h_pole = double_inverted_pendulum_dynamics.l/2*(cs.cos(theta_pole[0]) + cs.cos(theta_pole[1]))
stage.add_objective(-100*h_pole, sp.t0, sp.mid, sp.tf) # we want the pole to be upright
stage.add_objective(cs.sumsqr(dtheta_pole), sp.t0, sp.mid, sp.tf) # we want the pole to be upright

# Define the constraints
stage.at_t0().subject_to(theta_pole == theta_pole_0)
stage.at_t0().subject_to(dtheta_pole == dtheta_pole_0)

# pick a solver
ocp.solver("fatrop", {'post_expand':True, 'jit':True}, {}) # fatrop/ipopt

# put the solver inside a function
func = ocp.to_function("double_pendulum", [theta_pole_0, dtheta_pole_0], [ocp.at_t0(F), ocp.sample(theta_pole)[1]])

# solve the ocp
u0_sol, theta_pol_sol = func(np.array([0.1, 0.0]), np.array([0.0, 0.0]))
mechanism.animate(theta_pole, theta_pol_sol)