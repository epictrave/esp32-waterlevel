
#ifndef WATER_LEVEL_COMMON_H
#define WATER_LEVEL_COMMON_H

typedef enum WATER_LEVEL_STATE {
  NONE_WATER_LEVEL,
  LACK_WATER_LEVEL,
  GOOD_WATER_LEVEL,
  FLOOD_WATER_LEVEL,
  ERROR_WATER_LEVEL
} WaterLevelState;

#endif /** WATER_LEVEL_COMMON_H */