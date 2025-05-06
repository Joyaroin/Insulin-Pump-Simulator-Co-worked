#ifndef DEFS_H
#define DEFS_H

/*
 * Config
 */
#define MS_BETWEEN_TIMESTEPS 250
#define PERSON_INITIAL_BLOOD_GLUCOSE 5.0
#define SIMULATED_MINS_PER_TIMESTEP 5
#define MAX_SEGMENTS 3

// Time
#define MINS_PER_DAY (HOURS_PER_DAY*MINS_PER_HOUR)
#define MINS_PER_HOUR 60
#define HOURS_PER_DAY 24

// Init settings
#define DEFAULT_PROFILE_NAME ""
#define DEFAULT_LOCKSCREEN_PIN "1234"

#define DEFAULT_BASAL_INSULIN 1.0       //
#define DEFAULT_CARB_RATIO 1          // insulin units per X grams carbs
#define DEFAULT_CORRECTION_FACTOR 1.0   // insulin units per mmol/L
#define DEFAULT_TARGET_GLUCOSE 5.0      // mmol/L

#define INSULIN_FULL_DECAY_MINS 400

// Alert thresholds
#define LOW_INSULIN_THRESHOLD 30
#define LOW_BLOOD_GLUCOSE 3.9
#define HIGH_BLOOD_GLUCOSE 10.0

#endif // DEFS_H
