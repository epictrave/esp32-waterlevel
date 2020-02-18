// Copyright (c) Imaination Garden. Inc. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "water_level.h"

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

static const char *TAG = "water level";
static WaterLevel *water_levels = NULL;
static size_t water_level_num = 0;

esp_err_t water_level_init(int num) {
  if (num <= 0) {
    ESP_LOGE(TAG, "Num must be bigger than 0");
    return ESP_ERR_INVALID_ARG;
  }
  water_level_num = num;
  if (water_levels != NULL) {
    free(water_levels);
    water_levels = NULL;
  }
  water_levels = (WaterLevel *)malloc(sizeof(WaterLevel) * water_level_num);
  for (int i = 0; i < water_level_num; i++) {
    water_levels[i].last_level = -1;
    water_levels[i].level = -1;
    water_levels[i].last_state = NONE_WATER_LEVEL;
    water_levels[i].state = NONE_WATER_LEVEL;
    water_levels[i].max_level = -1;
    water_levels[i].min_level = 100;
    water_levels[i].pins = NULL;
    water_levels[i].pin_num = 0;
    water_levels[i].outlet_id = -1;
    memset(water_levels[i].control_device_id, 0,
           sizeof(water_levels[i].control_device_id));
    ESP_LOGI(TAG, "water level #%d is initalized.", i);
  }
  return ESP_OK;
}

esp_err_t water_level_set_pins(int index, gpio_num_t gpios[], int gpio_num) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than or equal to and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }
  if (gpio_num <= 0) {
    ESP_LOGE(TAG, "GPIO num must be bigger than 0");
    return ESP_ERR_INVALID_ARG;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  selected_water_level->pin_num = gpio_num;

  if (selected_water_level->pins != NULL) {
    free(selected_water_level->pins);
    selected_water_level->pins = NULL;
  }
  selected_water_level->pins =
      (gpio_num_t *)malloc(sizeof(gpio_num_t) * selected_water_level->pin_num);

  for (int i = 0; i < selected_water_level->pin_num; i++) {
    selected_water_level->pins[i] = gpios[i];
    gpio_pad_select_gpio(selected_water_level->pins[i]);
    ESP_ERROR_CHECK(
        gpio_set_direction(selected_water_level->pins[i], GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(
        gpio_set_pull_mode(selected_water_level->pins[i], GPIO_PULLDOWN_ONLY));
  }
  return ESP_OK;
}

esp_err_t water_level_set_min_level(int index, int min_level) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG,
             "IIndex must be bigger than or equal to 0 and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }

  if (min_level < 0) {
    ESP_LOGE(TAG, "Min level must be bigger than 0");
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  selected_water_level->min_level = min_level;
  return ESP_OK;
}

int water_level_get_min_level(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG,
             "Index must be bigger than or equal to 0 and smaller than %d.",
             water_level_num);
    return -1;
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->min_level;
}

esp_err_t water_level_set_max_level(int index, int max_level) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }

  if (max_level < 0) {
    ESP_LOGE(TAG, "Max level must be bigger than 0.");
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  selected_water_level->max_level = max_level;
  return ESP_OK;
}

int water_level_get_max_level(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return -1;
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->max_level;
}

esp_err_t water_level_set_outlet_id(int index, int outlet_id) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  selected_water_level->outlet_id = outlet_id;
  return ESP_OK;
}

int water_level_get_outlet_id(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return -1;
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->outlet_id;
}

esp_err_t water_level_set_control_device_id(int index,
                                            const char *control_device_id) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }

  WaterLevel *selected_water_level = &(water_levels[index]);

  memset(selected_water_level->control_device_id, 0,
         sizeof(selected_water_level->control_device_id));
  if (control_device_id != NULL) {
    strncpy(selected_water_level->control_device_id, control_device_id,
            sizeof(control_device_id) + 1);
  }
  return ESP_OK;
}

char *water_level_get_control_device_id(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return NULL;
  }

  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->control_device_id;
}

esp_err_t water_level_set_level(int index, int level) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }
  if (level < 0) {
    ESP_LOGE(TAG, "Level must be bigger than 0.");
    return ESP_ERR_INVALID_ARG;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  selected_water_level->last_level = selected_water_level->level;
  selected_water_level->level = level;
  return ESP_OK;
}

int water_level_get_level(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return -1;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->level;
}

esp_err_t water_level_set_state(int index, WaterLevelState state) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return ESP_ERR_INVALID_ARG;
  }
  if (state < 0) {
    ESP_LOGE(TAG, "state must be bigger than 0.");
    return ESP_ERR_INVALID_ARG;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  selected_water_level->last_state = selected_water_level->state;
  selected_water_level->state = state;
  return ESP_OK;
}

WaterLevelState water_level_get_state(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return NONE_WATER_LEVEL;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->state;
}

bool water_level_is_level_changed(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return false;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->level != selected_water_level->last_level;
}

bool water_level_is_state_changed(int index) {
  if (index < 0 || water_level_num <= index) {
    ESP_LOGE(TAG, "Index must be bigger than 0 and smaller than %d.",
             water_level_num);
    return false;
  }
  WaterLevel *selected_water_level = &(water_levels[index]);
  return selected_water_level->state != selected_water_level->last_state;
}

void water_level_run(void) {
  if (water_level_num <= 0) {
    ESP_LOGE(TAG, "Water level is not initialized.");
    return;
  }

  for (int index = 0; index < water_level_num; index++) {
    WaterLevel *selected_water_level = &(water_levels[index]);
    int temp_level = 0;

    for (int gpio_index = 0; gpio_index < selected_water_level->pin_num;
         gpio_index++) {
      if (gpio_get_level(selected_water_level->pins[gpio_index]) == HIGH) {
        temp_level = gpio_index + 1;
      }
    }

    water_level_set_level(index, temp_level);
    int min_level = selected_water_level->min_level;
    int max_level = selected_water_level->max_level;
    int level = selected_water_level->level;

    if (level < min_level) {
      water_level_set_state(index, LACK_WATER_LEVEL);
    } else if (min_level <= level && level < max_level) {
      water_level_set_state(index, GOOD_WATER_LEVEL);
    } else if (max_level <= level) {
      water_level_set_state(index, FLOOD_WATER_LEVEL);
    } else {
      water_level_set_state(index, ERROR_WATER_LEVEL);
    }

    ESP_LOGI(TAG, "water level #%d level : %d state: %d", index,
             water_level_get_level(index), water_level_get_state(index));

    if (water_level_is_level_changed(index)) {
      water_level_add_message_number(index, "level",
                                     water_level_get_level(index));
    }

    if (water_level_is_state_changed(index)) {
      water_level_add_message_number(index, "state",
                                     water_level_get_state(index));
    }
  }
}

void water_level_parse_from_json(const char *json,
                                 DEVICE_TWIN_STATE update_state) {
  JSON_Value *root_value = json_parse_string(json);
  JSON_Object *root_object = json_value_get_object(root_value);

  for (int i = 0; i < water_level_num; i++) {
    char water_level_property[30];
    if (update_state == UPDATE_PARTIAL) {
      snprintf(water_level_property, sizeof(water_level_property),
               "waterLevel.%d", i);
    } else if (update_state == UPDATE_COMPLETE) {
      snprintf(water_level_property, sizeof(water_level_property),
               "desired.waterLevel.%d", i);
    }

    JSON_Object *json_water_level =
        json_object_dotget_object(root_object, water_level_property);

    if (json_object_get_value(json_water_level, "minLevel") != NULL) {
      int max_level = json_object_get_number(json_water_level, "minLevel");
      water_level_set_min_level(i, max_level);
      water_level_add_message_number(i, "minLevel",
                                     water_level_get_min_level(i));
    }

    if (json_object_get_value(json_water_level, "maxLevel") != NULL) {
      int max_level = json_object_get_number(json_water_level, "maxLevel");
      water_level_set_max_level(i, max_level);
      water_level_add_message_number(i, "maxLevel",
                                     water_level_get_max_level(i));
    }

    if (json_object_get_value(json_water_level, "outletId") != NULL) {
      int outlet_id = json_object_get_number(json_water_level, "outletId");
      water_level_set_outlet_id(i, outlet_id);
      water_level_add_message_number(i, "outletId",
                                     water_level_get_outlet_id(i));
    }

    if (json_object_get_value(json_water_level, "controlDeviceId") != NULL) {
      const char *control_device_id =
          json_object_get_string(json_water_level, "controlDeviceId");
      if (control_device_id == NULL)
        ESP_LOGI(TAG, "null");
      // TODO 이거 됨
      else {
        ESP_LOGI(TAG, "not null");
      }
      water_level_set_control_device_id(i, control_device_id);
      water_level_add_message_string(i, "controlDeviceId",
                                     water_level_get_control_device_id(i));
    }
  }
  json_value_free(root_value);
}
void water_level_add_message_number(int water_level_index, char *name,
                                    double value) {
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);

  char water_level_name[20];
  snprintf(water_level_name, sizeof(water_level_name), "waterLevel-%d",
           water_level_index);
  json_object_set_string(root_object, "messageType", "sensor");
  json_object_set_string(root_object, "sensorName", water_level_name);
  json_object_set_string(root_object, "name", name);
  json_object_set_number(root_object, "value", value);

  char *result = json_serialize_to_string(root_value);
  queue_message_add_message(result);

  json_value_free(root_value);
  free(result);
}

void water_level_add_message_string(int water_level_index, char *name,
                                    char *value) {
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);

  char water_level_name[20];
  snprintf(water_level_name, sizeof(water_level_name), "waterLevel-%d",
           water_level_index);
  json_object_set_string(root_object, "messageType", "sensor");
  json_object_set_string(root_object, "sensorName", water_level_name);
  json_object_set_string(root_object, "name", name);
  json_object_set_string(root_object, "value", value);

  char *result = json_serialize_to_string(root_value);
  queue_message_add_message(result);

  json_value_free(root_value);
  free(result);
}