/************************ TRICK HEADER *************************
PURPOSE: (CubeSat translational orbit with independent perturbation models.)
LIBRARY DEPENDENCIES: ((orbit/src/Orbit.cpp))
***************************************************************/
#ifndef LEARNING_ORBIT_HH
#define LEARNING_ORBIT_HH
class Orbit {
public:
    double pos[3]; /* m Geocentric inertial position, axes approximately ICRF */
    double vel[3]; /* m/s Inertial velocity */
    double acc[3]; /* m/s2 Total acceleration */
    double mu; /* m3/s2 Earth GM */
    double radius; /* m Spherical reference radius */
    double j2; /* -- Unnormalized second zonal harmonic */
    bool use_j2; /* -- Include J2 */
    bool use_drag; /* -- Include co-rotating exponential atmosphere */
    bool use_srp; /* -- Include solar radiation pressure and eclipses */
    bool use_sun; /* -- Include differential solar gravity */
    bool use_moon; /* -- Include differential lunar gravity */
    double mass; /* kg Constant spacecraft mass */
    double drag_area; /* m2 Constant effective cross section for drag */
    double srp_area; /* m2 Constant effective solar radiation area */
    double cd; /* -- Drag coefficient */
    double cr; /* -- Cannonball radiation coefficient, 0..2 */
    double rho_ref; /* kg/m3 Assumed nominal atmosphere reference density */
    double density_scale; /* -- Density sensitivity multiplier, not space weather index */
    double atmosphere_h_ref; /* m Reference spherical altitude */
    double scale_height; /* m Exponential scale height */
    double earth_omega; /* rad/s Atmosphere rotation about fixed inertial Z */
    double solar_pressure; /* N/m2 Radiation pressure at one astronomical unit */
    double epoch_offset_s; /* s Offset from bundled epoch 2026-10-16 00:00 TDB */
    double altitude0; /* m Initial perigee altitude above reference sphere */
    double eccentricity; /* -- Initial osculating eccentricity */
    double inclination_deg; /* -- Initial inclination in degrees */
    double raan_deg; /* -- Initial ascending node longitude in degrees */
    double arg_perigee_deg; /* -- Argument of perigee in degrees */
    double true_anomaly_deg; /* -- Initial true anomaly in degrees */
    double altitude; /* m Spherical altitude, not geodetic */
    double speed; /* m/s Inertial speed */
    double energy; /* m2/s2 Earth-specific energy including J2 */
    double energy_change; /* -- Physical plus numerical relative energy change */
    double energy_balance_error; /* -- Residual after integrating perturbation work */
    double initial_energy; /* m2/s2 Initial Earth-specific energy */
    double perturbation_work; /* m2/s2 Integrated v dot non-Earth acceleration */
    double perturbation_power; /* m2/s3 Work rate of drag, SRP and third bodies */
    double period; /* s Initial two-body orbital period */
    double density; /* kg/m3 Local density, zero if drag disabled */
    double illumination; /* -- Visible solar disk fraction, 0..1 */
    double ballistic_coefficient; /* kg/m2 mass divided by Cd area; zero when Cd area zero */
    double central_acc[3]; /* m/s2 Central gravity */
    double j2_acc[3]; /* m/s2 J2 correction */
    double drag_acc[3]; /* m/s2 Drag acceleration */
    double srp_acc[3]; /* m/s2 Solar radiation acceleration */
    double sun_acc[3]; /* m/s2 Differential solar gravity */
    double moon_acc[3]; /* m/s2 Differential lunar gravity */
    double sun_distance; /* m Distance from spacecraft to Sun */
    double sun_pos[3]; /* m Geocentric Sun position */
    double moon_pos[3]; /* m Geocentric Moon position */
    double central_magnitude; /* m/s2 Central acceleration magnitude */
    double j2_magnitude; /* m/s2 J2 acceleration magnitude */
    double drag_magnitude; /* m/s2 Drag acceleration magnitude */
    double srp_magnitude; /* m/s2 SRP acceleration magnitude */
    double sun_magnitude; /* m/s2 Solar acceleration magnitude */
    double moon_magnitude; /* m/s2 Lunar acceleration magnitude */
    int default_data();
    int initialize();
    int derivatives();
    int evaluate_forces(double time);
    int integrate_state();
    int diagnostics();
};
#endif
