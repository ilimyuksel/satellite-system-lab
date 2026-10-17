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
             "low_power", "power_limited", "eps.capacity_soh", "eps.effective_capacity_wh",
             "eps.ocv_empty_v", "eps.ocv_full_v", "eps.internal_resistance", "eps.min_terminal_v", "eps.max_terminal_v",
             "eps.max_charge_a", "eps.max_discharge_a", "eps.max_charge_w", "eps.max_discharge_w",
             "eps.charge_efficiency", "eps.discharge_efficiency", "eps.regulation_efficiency", "eps.panel_health",
             "eps.base_load_w", "eps.cutoff_soc", "eps.recovery_soc", "eps.ocv_v", "eps.terminal_v",
             "eps.battery_current_a", "eps.ohmic_heat_w", "eps.converter_loss_w", "eps.discharge_limit_w", "eps.charge_limit_w",
             "payload.active_w", "payload.standby_w", "payload.first_on_s", "payload.cycle_s", "payload.on_duration_s",
             "payload.data_rate_mbps", "payload.heat_fraction", "payload.generated_data_mbit", "payload.heat_w"):
    rec.add_variable("dyn.spacecraft."+name)
for name in ("conductance_battery", "conductance_payload", "solar_area", "absorptivity", "radiating_area", "emissivity",
             "earth_ir_w_m2", "earth_view_factor", "space_k", "heater_enabled", "heater_on_k", "heater_off_k", "heater_rating_w",
             "heater_requested", "heater_served_w", "solar_absorbed_w", "earth_absorbed_w", "radiated_w",
             "body_to_battery_w", "body_to_payload_w", "net_energy_j", "balance_error_j", "heater_energy_wh"):
    rec.add_variable("dyn.spacecraft.thermal."+name)
for name in ("mass", "cp", "initial_k", "temperature_k", "temperature_c", "internal_w"):
    for axis in range(3):rec.add_variable(f"dyn.spacecraft.thermal.{name}[{axis}]")
trick.add_data_record_group(rec, trick.DR_Buffer)
rec.enable()
