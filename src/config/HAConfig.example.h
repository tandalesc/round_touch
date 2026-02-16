// Copy this file to HAConfig.h and fill in your entity IDs.
// HAConfig.h is gitignored and will not be committed.

#ifndef _HA_CONFIG_H_
#define _HA_CONFIG_H_

// Weather entity
#define HA_WEATHER_ENTITY "weather.forecast_home"

// Presence sensors (binary_sensor entities)
#define HA_PRESENCE_1_ENTITY "binary_sensor.person1_presence"
#define HA_PRESENCE_1_LABEL "Person 1"
#define HA_PRESENCE_2_ENTITY "binary_sensor.person2_presence"
#define HA_PRESENCE_2_LABEL "Person 2"

// Light/switch entities
#define HA_LIGHT_1 "light.living_room"
#define HA_LIGHT_2 "light.bedroom"
#define HA_LIGHT_3 "light.kitchen"
#define HA_LIGHT_4 "switch.outlet_1"

#endif // _HA_CONFIG_H_
