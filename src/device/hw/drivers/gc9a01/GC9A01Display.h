#ifndef _GC9A01_DISPLAY_H_
#define _GC9A01_DISPLAY_H_

#include <Arduino.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

#include "BoardConfig.h"
#include "device/IDisplay.h"
#include "esp_lcd_gc9a01.h"
#include "draw/sw/lv_draw_sw.h"

#define GC9A01_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10)

class GC9A01Display : public IDisplay {
private:
  esp_lcd_panel_handle_t _panel = nullptr;
  esp_lcd_panel_io_handle_t _io = nullptr;
  lv_display_t *_disp = nullptr;
  uint16_t *_buf1 = nullptr;
  uint16_t *_buf2 = nullptr;

  static void flushCb(lv_display_t *disp, const lv_area_t *area, uint8_t *px) {
    auto *self = (GC9A01Display *)lv_display_get_user_data(disp);
    // Byte-swap RGB565 for SPI big-endian
    uint16_t *p = (uint16_t *)px;
    uint32_t num_px = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    for (uint32_t i = 0; i < num_px; i++) {
      p[i] = (p[i] >> 8) | (p[i] << 8);
    }
    esp_lcd_panel_draw_bitmap(self->_panel, area->x1, area->y1,
                              area->x2 + 1, area->y2 + 1, px);
    lv_display_flush_ready(disp);
  }

public:
  GC9A01Display() {}
  ~GC9A01Display() {
    free(_buf1);
    free(_buf2);
    if (_panel) esp_lcd_panel_del(_panel);
    if (_io) esp_lcd_panel_io_del(_io);
  }

  void init() override {
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    spi_bus_config_t buscfg = {};
    buscfg.sclk_io_num = TFT_SCLK;
    buscfg.mosi_io_num = TFT_MOSI;
    buscfg.miso_io_num = TFT_MISO;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = GC9A01_BUF_SIZE * sizeof(uint16_t);
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.dc_gpio_num = TFT_DC;
    io_config.cs_gpio_num = TFT_CS;
    io_config.pclk_hz = 40 * 1000 * 1000;
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    io_config.spi_mode = 0;
    io_config.trans_queue_depth = 1;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
        (esp_lcd_spi_bus_handle_t)SPI3_HOST, &io_config, &_io));

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.reset_gpio_num = TFT_RES;
    panel_config.rgb_endian = LCD_RGB_ENDIAN_RGB;
    panel_config.bits_per_pixel = 16;
    ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(_io, &panel_config, &_panel));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(_panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(_panel, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(_panel, true));
  }

  lv_display_t *initLVGL() override {
    _disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_user_data(_disp, this);
    lv_display_set_flush_cb(_disp, flushCb);
    lv_display_set_color_format(_disp, LV_COLOR_FORMAT_RGB565);

    size_t buf_bytes = GC9A01_BUF_SIZE * sizeof(uint16_t);
    _buf1 = (uint16_t *)heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA);
    _buf2 = (uint16_t *)heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA);
    lv_display_set_buffers(_disp, _buf1, _buf2, buf_bytes,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    return _disp;
  }

  int width() override { return SCREEN_WIDTH; }
  int height() override { return SCREEN_HEIGHT; }
};

#endif // _GC9A01_DISPLAY_H_
