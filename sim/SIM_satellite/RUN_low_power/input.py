exec(open("RUN_nominal/input.py").read())
# Deliberately energy-poor operating scenario, NOT a fault-injection implementation.
dyn.spacecraft.eps.initial_soc = 0.22
dyn.spacecraft.eps.panel_area = 0.025
