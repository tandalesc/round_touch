#ifndef _RGB_PANEL_DISPLAY_H_
#define _RGB_PANEL_DISPLAY_H_

#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

#include "BoardConfig.h"
#include "device/IDisplay.h"
#include "draw/sw/lv_draw_sw.h"

class RGBPanelDisplay : public IDisplay {
private:
  esp_lcd_panel_handle_t _panel = nullptr;
  uint16_t *_buf1 = nullptr;
  uint16_t *_buf2 = nullptr;
  uint16_t *_rotBuf = nullptr;
  static lv_display_t *_lvDisp;

  static void flushCb(lv_display_t *disp, const lv_area_t *area, uint8_t *px) {
    auto *self = (RGBPanelDisplay *)lv_display_get_user_data(disp);
    const lv_area_t *flush_area = area;
    lv_area_t rotated_area;

    // Software 180° rotation — RGB panels have no MADCTL register,
    // so we reverse the pixel data and mirror the area coordinates.
    if (lv_display_get_rotation(disp) == LV_DISPLAY_ROTATION_180) {
      lv_color_format_t cf = lv_display_get_color_format(disp);
      int32_t w = lv_area_get_width(area);
      int32_t h = lv_area_get_height(area);
      uint32_t stride = w * lv_color_format_get_size(cf);

      lv_draw_sw_rotate(px, self->_rotBuf, w, h, stride, stride,
                        LV_DISPLAY_ROTATION_180, cf);
      px = (uint8_t *)self->_rotBuf;

      rotated_area = *area;
      lv_display_rotate_area(disp, &rotated_area);
      flush_area = &rotated_area;
    }

    esp_lcd_panel_draw_bitmap(self->_panel, flush_area->x1, flush_area->y1,
                              flush_area->x2 + 1, flush_area->y2 + 1, px);
  }

  // ISR: draw_bitmap finished copying into the framebuffer.
  // Signal LVGL that the draw buffer can be reused.
  static bool onColorTransDone(esp_lcd_panel_handle_t panel,
                               const esp_lcd_rgb_panel_event_data_t *edata,
                               void *user_ctx) {
    lv_display_flush_ready(_lvDisp);
    return false;
  }

public:
  RGBPanelDisplay() {}
  ~RGBPanelDisplay() {
    free(_rotBuf);
    free(_buf1);
    free(_buf2);
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
    panel_config.timings.vsync_back_porch = 16;
    panel_config.timings.vsync_front_porch = 16;
    panel_config.timings.flags.pclk_active_neg = 1;

    panel_config.flags.fb_in_psram = 1;

    // Bounce buffers in internal SRAM decouple LCD DMA from PSRAM bus.
    // The DMA reads from these fast SRAM buffers instead of directly
    // from PSRAM, preventing tearing and corruption from bus contention.
    panel_config.bounce_buffer_size_px = SCREEN_WIDTH * 10;

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(_panel));

    // Register ISR callback: fires when draw_bitmap finishes copying
    // the LVGL draw buffer into the framebuffer, so LVGL can start
    // rendering the next dirty area into the other draw buffer.
    esp_lcd_rgb_panel_event_callbacks_t cbs = {};
    cbs.on_color_trans_done = onColorTransDone;
    ESP_ERROR_CHECK(
        esp_lcd_rgb_panel_register_event_callbacks(_panel, &cbs, nullptr));
  }

  lv_display_t *initLVGL() override {
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    _lvDisp = disp;
    lv_display_set_user_data(disp, this);
    lv_display_set_flush_cb(disp, flushCb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    // Two LVGL draw buffers in PSRAM, each 1/4 screen. LVGL renders
    // into one while the other is being copied to the framebuffer
    // (completion signaled by on_color_trans_done ISR).
    size_t buf_size = SCREEN_WIDTH * SCREEN_HEIGHT / 4 * sizeof(uint16_t);
    _buf1 = (uint16_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    _buf2 = (uint16_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    _rotBuf = (uint16_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    lv_display_set_buffers(disp, _buf1, _buf2, buf_size,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    return disp;
  }

  int width() override { return SCREEN_WIDTH; }
  int height() override { return SCREEN_HEIGHT; }
};

lv_display_t *RGBPanelDisplay::_lvDisp = nullptr;

#endif // _RGB_PANEL_DISPLAY_H_
