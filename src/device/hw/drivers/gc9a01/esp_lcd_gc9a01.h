/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_LCD_GC9A01_VER_MAJOR 2
#define ESP_LCD_GC9A01_VER_MINOR 0
#define ESP_LCD_GC9A01_VER_PATCH 4

typedef struct {
    int cmd;
    const void *data;
    size_t data_bytes;
    unsigned int delay_ms;
} gc9a01_lcd_init_cmd_t;

typedef struct {
    const gc9a01_lcd_init_cmd_t *init_cmds;
    uint16_t init_cmds_size;
} gc9a01_vendor_config_t;

esp_err_t esp_lcd_new_panel_gc9a01(const esp_lcd_panel_io_handle_t io,
                                   const esp_lcd_panel_dev_config_t *panel_dev_config,
                                   esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif
