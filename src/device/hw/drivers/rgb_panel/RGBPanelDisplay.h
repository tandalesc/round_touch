#ifndef _RGB_PANEL_DISPLAY_H_
#define _RGB_PANEL_DISPLAY_H_

#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

#include "device/IDisplay.h"

class RGBPanelDisplay : public IDisplay {
private:
  esp_lcd_panel_handle_t _panel = nullptr;

public:
  RGBPanelDisplay() {}
  ~RGBPanelDisplay() {
    if (_panel) {
      esp_lcd_panel_del(_panel);
    }
  }

  void init() override {
    esp_lcd_rgb_panel_config_t panel_config = {};
    panel_config.data_width = 16;
    panel_config.clk_src = LCD_CLK_SRC_DEFAULT;
    panel_config.disp_gpio_num = -1;
    panel_config.pclk_gpio_num = LCD_PCLK;
    panel_config.vsync_gpio_num = LCD_VSYNC;
    panel_config.hsync_gpio_num = LCD_HSYNC;
    panel_config.de_gpio_num = LCD_DE;

    // RGB565 data pins: B[3:7], G[2:7], R[3:7]
    panel_config.data_gpio_nums[0] = LCD_B3;
    panel_config.data_gpio_nums[1] = LCD_B4;
    panel_config.data_gpio_nums[2] = LCD_B5;
    panel_config.data_gpio_nums[3] = LCD_B6;
    panel_config.data_gpio_nums[4] = LCD_B7;
    panel_config.data_gpio_nums[5] = LCD_G2;
    panel_config.data_gpio_nums[6] = LCD_G3;
    panel_config.data_gpio_nums[7] = LCD_G4;
    panel_config.data_gpio_nums[8] = LCD_G5;
    panel_config.data_gpio_nums[9] = LCD_G6;
    panel_config.data_gpio_nums[10] = LCD_G7;
    panel_config.data_gpio_nums[11] = LCD_R3;
    panel_config.data_gpio_nums[12] = LCD_R4;
    panel_config.data_gpio_nums[13] = LCD_R5;
    panel_config.data_gpio_nums[14] = LCD_R6;
    panel_config.data_gpio_nums[15] = LCD_R7;

    panel_config.timings.pclk_hz = 16000000;
    panel_config.timings.h_res = SCREEN_WIDTH;
    panel_config.timings.v_res = SCREEN_HEIGHT;
    panel_config.timings.hsync_pulse_width = 4;
    panel_config.timings.hsync_back_porch = 8;
    panel_config.timings.hsync_front_porch = 8;
    panel_config.timings.vsync_pulse_width = 4;
    panel_config.timings.vsync_back_porch = 8;
    panel_config.timings.vsync_front_porch = 8;
    panel_config.timings.flags.pclk_active_neg = 1;

    panel_config.flags.fb_in_psram = 1;
    panel_config.bounce_buffer_size_px = 10 * SCREEN_WIDTH;

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(_panel));
  }

  esp_lcd_panel_handle_t panel() { return _panel; }
  int width() override { return SCREEN_WIDTH; }
  int height() override { return SCREEN_HEIGHT; }
};

#endif // _RGB_PANEL_DISPLAY_H_
