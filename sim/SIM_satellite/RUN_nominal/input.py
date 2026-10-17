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

# Electrical pack model (synthetic OCV versus ENERGY fraction, no cell calibration).
eps = dyn.spacecraft.eps
eps.capacity_soh = 1.0             # fixed remaining capacity ratio
eps.ocv_empty_v = 12.0             # pack-level open circuit voltage, V
eps.ocv_full_v = 16.8
eps.internal_resistance = 0.08     # pack resistance, ohm
eps.min_terminal_v = 11.2          # discharge cutoff, V
eps.max_terminal_v = 16.8          # charge ceiling, V
eps.max_charge_a = 2.0
eps.max_discharge_a = 3.0
eps.max_charge_w = 30.0            # converter limits
eps.max_discharge_w = 40.0
eps.charge_efficiency = 0.95
eps.discharge_efficiency = 0.95
eps.regulation_efficiency = 0.9
eps.panel_health = 1.0             # fixed available panel output fraction
eps.base_load_w = 8.0
eps.cutoff_soc = 0.20
eps.recovery_soc = 0.35

dyn.spacecraft.payload.data_rate_mbps = 2.0
dyn.spacecraft.payload.heat_fraction = 0.9

# Three-node thermal network: body, battery, payload. Masses total 12 kg.
th = dyn.spacecraft.thermal
th.mass = [8.0, 2.0, 2.0]                 # kg
th.cp = [900.0, 1000.0, 800.0]            # J/(kg K)
th.initial_k = [293.15, 293.15, 293.15]   # K
th.conductance_battery = 0.6             # W/K
th.conductance_payload = 0.8             # W/K
th.solar_area = 0.06                    # m² non-PV projected body area
th.absorptivity = 0.6
th.radiating_area = 0.25                # m²
th.emissivity = 0.75
th.earth_ir_w_m2 = 237.0                 # fixed scenario assumption, not sampled Earth map
th.earth_view_factor = 0.3
th.space_k = 3.0
th.heater_enabled = True
th.heater_on_k = 278.15                  # 5 °C
th.heater_off_k = 283.15                 # 10 °C
th.heater_rating_w = 8.0
