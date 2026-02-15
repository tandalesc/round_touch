# round_touch

Multi-board ESP32-S3 touch display framework with a declarative UI layer built on LVGL.

## Supported Boards

| Board | Display | Touch | Resolution |
|-------|---------|-------|------------|
| [MakerFabs ESP32-S3 Round 1.28"][makerfabs] | GC9A01 (SPI) | CST816S (I2C) | 240x240 |
| [Waveshare ESP32-S3-Touch-LCD-7][waveshare] | RGB parallel | GT911 (I2C) | 800x480 |
| Desktop simulator | SDL2 | Mouse/keyboard | Configurable |

## Prerequisites

- [uv](https://docs.astral.sh/uv/) (Python package manager, used to run PlatformIO)
- SDL2 (simulator only): `brew install sdl2` on macOS
- CMake 3.16+ (simulator only)

## Setup

```bash
git clone --recursive https://github.com/tandalesc/round_touch.git
cd round_touch
uv sync
```

The `--recursive` flag pulls the LVGL v9.2.2 submodule at `lib/lvgl`. If you already cloned without it:

```bash
git submodule update --init --recursive
```

## Build & Flash

### MakerFabs 1.28" Round

```bash
# Build + upload + monitor serial
uv run pio run -e makerfabs_round_128 -t upload && uv run pio device monitor

# Build only
uv run pio run -e makerfabs_round_128
```

### Waveshare 7" LCD

```bash
# Build + upload + monitor serial
uv run pio run -e waveshare_s3_lcd_7 -t upload && uv run pio device monitor

# Build only
uv run pio run -e waveshare_s3_lcd_7
```

### Simulator

The simulator uses SDL2 and CMake. Default resolution is 240x240 (MakerFabs).

```bash
# Build (240x240 default)
cd simulator/build && cmake .. && make -j8

# Build at Waveshare resolution
cd simulator/build && cmake .. -DSCREEN_WIDTH=800 -DSCREEN_HEIGHT=480 && make -j8

# Run
./round_touch_sim
```

## Architecture

```
Device                          Board abstraction (IDisplay, ITouch, IStorage)
  hw/drivers/                   Per-board hardware drivers
    gc9a01/                     SPI display via esp_lcd (MakerFabs)
    rgb_panel/                  RGB parallel display via esp_lcd (Waveshare)
    gt911/                      I2C touch (Waveshare)
    cst816s/                    I2C touch (MakerFabs)
    ch422g/                     I/O expander (Waveshare)

Application(&device)
  Workflow                      State machine for screen navigation
  Interface                     Drives LVGL rendering + input dispatch
    ComponentManager            Creates/destroys component trees on state change
    Components                  Declarative UI via E() macro DSL
      FillScreen                Full-screen flex container with background color
      FlexLayout                Row/Column flex with alignment and gap
      Text                      LVGL label with size and color
      TouchNavigation           Swipe/tap → state transition rules

Events
  EventQueue<Event>             Pub/sub for touch and workflow events
```

### Declarative UI

Screens are defined as component trees using the `E()` macro:

```cpp
E(FillScreen, {.color = 0x00FF00},
  E(TouchNavigation, onSwipeUp(READY)),
  E(FlexLayout, {.type = LayoutType::Row, .align = Align::Center},
    E(Text, {.color = 0x000000}, "ECO Mode")
  )
)
```

## Board-Specific Notes

### MakerFabs

- Uses SPI3_HOST for the GC9A01 display (SPI2 causes DMA hangs)
- Requires double-buffered LVGL flush with RGB565 byte swap
- USB serial: `ARDUINO_USB_CDC_ON_BOOT=1`

### Waveshare

- CH422G I/O expander controls display backlight, touch reset, and SD CS
- IO5 (USB_SEL) must stay LOW or USB serial disconnects
- Wire (I2C) must be re-initialized after RGB panel init
- USB serial: `ARDUINO_USB_CDC_ON_BOOT=1` + `ARDUINO_USB_MODE=1`

[makerfabs]: https://github.com/Makerfabs/ESP32-S3-Round-SPI-TFT-with-Touch-1.28
[waveshare]: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7
