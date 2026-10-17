# Header-based readers allow adding diagnostics without changing column indices.
rec = trick.DRAscii("orbit")
rec.set_cycle(5.0)
rec.set_freq(trick.DR_Always)
for vector in ("pos", "vel", "central_acc", "j2_acc", "drag_acc", "srp_acc", "sun_acc", "moon_acc", "sun_pos", "moon_pos"):
    for axis in range(3):
        rec.add_variable(f"dyn.satellite.{vector}[{axis}]")
for name in ("altitude", "speed", "energy", "initial_energy", "energy_change", "energy_balance_error",
             "perturbation_work", "radius", "mu", "j2", "use_j2", "use_drag", "use_srp", "use_sun", "use_moon",
             "inclination_deg", "raan_deg", "arg_perigee_deg", "true_anomaly_deg", "altitude0", "eccentricity",
             "mass", "drag_area", "srp_area", "cd", "cr", "rho_ref", "atmosphere_h_ref", "scale_height", "density_scale",
             "earth_omega", "solar_pressure", "epoch_offset_s", "density", "illumination", "ballistic_coefficient",
             "central_magnitude", "j2_magnitude", "drag_magnitude", "srp_magnitude", "sun_magnitude", "moon_magnitude"):
    rec.add_variable("dyn.satellite."+name)
for name in ("eps.capacity_wh", "eps.panel_area", "eps.panel_efficiency", "eps.incidence_cosine",
             "eps.initial_soc", "eps.soc", "eps.energy_wh", "eps.solar_w", "eps.requested_w",
             "eps.served_w", "eps.unserved_w", "eps.charge_w", "eps.discharge_w", "eps.curtailed_w",
             "eps.loss_w", "eps.balance_error_wh", "eps.bus_residual_w", "eps.harvested_wh", "eps.supplied_wh",
             "payload.commanded", "payload.mode", "payload.consumed_w", "payload.active_seconds",
             "low_power", "power_limited"):
    rec.add_variable("dyn.spacecraft."+name)
trick.add_data_record_group(rec, trick.DR_Buffer)
rec.enable()
