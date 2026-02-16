#ifndef _HA_WEATHER_COMPONENT_H_
#define _HA_WEATHER_COMPONENT_H_

#include "application/Application.h"
#include "application/interface/components/types/StatefulComponent.h"
#include "application/services/HomeAssistant.h"

// Displays weather condition, temperature, and humidity from a HA weather entity.
class HAWeather : public StatefulComponent {
  const char *entityId;
  bool loading = true;
  String condition = "unknown";
  String temperature = "--";
  String tempUnit = "";
  String humidity = "--";

  lv_obj_t *condLabel = nullptr;
  lv_obj_t *tempLabel = nullptr;
  lv_obj_t *humLabel = nullptr;

  // Map weather condition to an LVGL symbol
  static const char *conditionIcon(const String &cond) {
    if (cond == "sunny" || cond == "clear-night") return LV_SYMBOL_IMAGE;
    if (cond == "cloudy" || cond == "partlycloudy") return LV_SYMBOL_CHARGE;
    if (cond == "rainy" || cond == "pouring") return LV_SYMBOL_TINT;
    if (cond == "snowy" || cond == "snowy-rainy") return LV_SYMBOL_TINT;
    if (cond == "lightning" || cond == "lightning-rainy") return LV_SYMBOL_POWER;
    if (cond == "windy" || cond == "windy-variant") return LV_SYMBOL_SHUFFLE;
    if (cond == "fog" || cond == "hail") return LV_SYMBOL_MINUS;
    return LV_SYMBOL_EYE_OPEN;
  }

public:
  HAWeather(const char *entityId) : entityId(entityId) {};

  void createWidgets(lv_obj_t *parent) override {
    lvObj = lv_obj_create(parent);
    lv_obj_remove_style_all(lvObj);
    lv_obj_set_size(lvObj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(lvObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lvObj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lvObj, 8, 0);
    lv_obj_set_style_pad_row(lvObj, 4, 0);

    condLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(condLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(condLabel, lv_color_hex(0xFAFAFA), 0);

    tempLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(tempLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(tempLabel, lv_color_hex(0xFAFAFA), 0);

    humLabel = lv_label_create(lvObj);
    lv_obj_set_style_text_font(humLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(humLabel, lv_color_hex(0xA1A1AA), 0);

    loading = true;
    update();

    lv_timer_t *t = lv_timer_create(deferredFetchCb, 100, this);
    lv_timer_set_repeat_count(t, 1);
  }

  void update() override {
    if (condLabel == nullptr) return;
    if (loading) {
      lv_label_set_text(condLabel, LV_SYMBOL_REFRESH " Loading...");
      lv_label_set_text(tempLabel, "");
      lv_label_set_text(humLabel, "");
      return;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "%s %s", conditionIcon(condition),
             condition.c_str());
    lv_label_set_text(condLabel, buf);

    snprintf(buf, sizeof(buf), "%s%s", temperature.c_str(), tempUnit.c_str());
    lv_label_set_text(tempLabel, buf);

    snprintf(buf, sizeof(buf), LV_SYMBOL_TINT " %s%% humidity", humidity.c_str());
    lv_label_set_text(humLabel, buf);
  }

private:
  static void deferredFetchCb(lv_timer_t *timer) {
    auto *self = static_cast<HAWeather *>(lv_timer_get_user_data(timer));
    self->refreshState();
  }

  void refreshState() {
    if (app == nullptr) return;
    HomeAssistant *ha = app->ha();
    if (ha == nullptr) {
      loading = false;
      condition = "unavailable";
      update();
      return;
    }

    condition = ha->getEntityState(entityId);
    temperature = ha->getEntityAttribute(entityId, "temperature");
    tempUnit = ha->getEntityAttribute(entityId, "temperature_unit");
    humidity = ha->getEntityAttribute(entityId, "humidity");
    loading = false;
    update();
  }
};

#endif // _HA_WEATHER_COMPONENT_H_
