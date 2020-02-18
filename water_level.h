// Copyright (c) Imaination Garden. Inc. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "stdlib.h"
#include "string.h"

#include "device_twin_state.h"
#include "parson.h"
#include "queue_message.h"
#include "water_level_common.h"

typedef struct WATER_LEVEL_TAG {
  gpio_num_t *pins;
  int pin_num;
  int level;
  int last_level;
  int min_level;
  int max_level;
  int outlet_id;
  char control_device_id[20];
  WaterLevelState state;
  WaterLevelState last_state;
} WaterLevel;

esp_err_t water_level_init(int num);
esp_err_t water_level_set_pins(int index, gpio_num_t gpios[], int num);

esp_err_t water_level_set_min_level(int index, int min_level);
int water_level_get_min_level(int index);

esp_err_t water_level_set_max_level(int index, int max_level);
int water_level_get_max_level(int index);

esp_err_t water_level_set_outlet_id(int index, int outlet_id);
int water_level_get_outlet_id(int index);

esp_err_t water_level_set_control_device_id(int index,
                                            const char *control_device_id);
char *water_level_get_control_device_id(int index);

esp_err_t water_level_set_level(int index, int level);
int water_level_get_level(int index);

esp_err_t water_level_set_state(int index, WaterLevelState state);
WaterLevelState water_level_get_state(int index);

bool water_level_is_level_changed(int index);
bool water_level_is_state_changed(int index);

void water_level_run(void);

void water_level_parse_from_json(const char *json,
                                 DEVICE_TWIN_STATE update_state);
void water_level_add_message_number(int water_level_index, char *name,
                                    double value);
void water_level_add_message_string(int water_level_index, char *name,
                                    char *value);

#ifdef __cplusplus
}
#endif

#endif /* WATER_LEVEL_H */
