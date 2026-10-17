# 12 kg CubeSat educational scenario; areas are constant effective areas.
# Epoch: 2026-10-16 00:00:00 TDB + epoch_offset_s, NOT local time / UTC.
dyn.satellite.epoch_offset_s = 0.0

# Six initial orbital elements (perigee altitude replaces semimajor axis).
dyn.satellite.altitude0 = 400000.0          # m, spherical perigee altitude
dyn.satellite.eccentricity = 0.0
dyn.satellite.inclination_deg = 51.6        # deg
dyn.satellite.raan_deg = 25.0               # deg
dyn.satellite.arg_perigee_deg = 0.0         # deg
dyn.satellite.true_anomaly_deg = 0.0        # deg

# Effective cannonball model; NOT an attitude-dependent box-and-wing model.
dyn.satellite.mass = 12.0                  # kg
dyn.satellite.drag_area = 0.06             # m^2, assumed effective projected area
dyn.satellite.srp_area = 0.12              # m^2, assumed effective illuminated area
dyn.satellite.cd = 2.2                     # assumed drag coefficient
dyn.satellite.cr = 1.3                     # assumed radiation coefficient

# Exponential atmosphere: nominal sensitivity experiment, not space-weather data.
dyn.satellite.rho_ref = 3.725e-12          # kg/m^3 at h_ref
dyn.satellite.atmosphere_h_ref = 400000.0  # m
dyn.satellite.scale_height = 58515.0       # m
dyn.satellite.density_scale = 1.0          # multiply density to explore uncertainty

dyn.satellite.use_j2 = True
dyn.satellite.use_drag = True
dyn.satellite.use_srp = True
dyn.satellite.use_sun = True
dyn.satellite.use_moon = True

# Six orbital states plus the integrated work of perturbing forces.
dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 7)
exec(open("record.py").read())
trick.stop(12000.0)                        # 200 min; bundled ephemeris covers 48 h

# First coupled subsystem demonstration; all values are synthetic assumptions.
dyn.spacecraft.eps.capacity_wh = 80.0
dyn.spacecraft.eps.initial_soc = 0.65
dyn.spacecraft.eps.panel_area = 0.12
dyn.spacecraft.eps.panel_efficiency = 0.28
dyn.spacecraft.eps.incidence_cosine = 0.7  # prescribed, NOT computed by ADCS
# Payload request begins at 600 s, repeats every 1800 s, lasts 900 s.
dyn.spacecraft.payload.active_w = 28.0
dyn.spacecraft.payload.standby_w = 2.0
dyn.spacecraft.payload.first_on_s = 600.0
dyn.spacecraft.payload.cycle_s = 1800.0
dyn.spacecraft.payload.on_duration_s = 900.0
