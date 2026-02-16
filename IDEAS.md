# Ideas & Improvements

Prioritized list of features and improvements for round_touch.
Each item includes context on what exists today and notes for implementation.

---

## Critical — Production Hardening

### P0. OTA Rollback

**Problem:** If a bad firmware is pushed via OTA, every device that updates is
bricked until USB reflash. There is no automatic recovery.

**Current code:**
- `OTAUpdate` in `src/application/services/OTAUpdate.cpp` uses Arduino `Update`
  library to write firmware. After `Update.end(true)`, the device calls
  `ESP.restart()`.
- No call to `esp_ota_mark_app_valid_cancel_rollback()` anywhere.
- ESP-IDF's bootloader supports automatic rollback: if the new app doesn't
  confirm itself as valid within N boots, the bootloader reverts to the
  previous partition.

**Approach:**
- Add `#include <esp_ota_ops.h>` to `Application.cpp`.
- Call `esp_ota_mark_app_valid_cancel_rollback()` at the end of
  `Application::init()` after everything has initialized successfully.
- That's it. If init crashes or hangs before reaching that line, the bootloader
  rolls back on next boot.
- Requires `CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y` in sdkconfig (check if
  pioarduino sets this by default, otherwise add to `platformio.ini`
  `board_build.cmake_extra_args`).

**Estimated scope:** 2-3 lines of code + build config verification.

---

### P1. Watchdog Timer

**Problem:** If the main loop hangs (blocked HTTP call with no timeout, I2C bus
lockup, LVGL deadlock), the device is completely unresponsive with no recovery
path except physical power cycle.

**Current code:**
- `Application::loop()` in `src/application/Application.cpp` runs:
  `pollEvent()` → `interface().loop()` → `delay(20)`.
- No watchdog registration. A blocking `_network->get()` call with a
  non-responsive server will hang the loop for the HTTP timeout duration
  (default ~15 seconds for HTTPClient).
- During that time: no touch input, no LVGL render, no OTA check.

**Approach:**
- Enable ESP-IDF Task Watchdog Timer (TWDT):
  ```cpp
  #include <esp_task_wdt.h>
  // In setup() or Application::init():
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 10000,  // 10 second timeout
      .idle_core_mask = 0,
      .trigger_panic = true,  // reboot on timeout
  };
  esp_task_wdt_reconfigure(&wdt_config);
  esp_task_wdt_add(NULL);  // add current task (loopTask)
  ```
- In `Application::loop()`, call `esp_task_wdt_reset()` each iteration.
- If `loop()` takes >10s (blocked network call), the watchdog reboots the
  device. Combined with OTA rollback (P0), this prevents permanent bricks.
- Tune timeout based on worst-case legitimate loop time. 10s is conservative.
- Simulator: stub out `esp_task_wdt_*` calls (they're ESP-IDF only).

**Estimated scope:** ~10 lines in Application + shim stub for simulator.

---

### P2. Timer millis() Overflow Fix

**Problem:** `Timer::running()` compares `millis() < timeout` where `timeout`
is an absolute future timestamp. `millis()` overflows after ~49.7 days
(uint32_t max). When it wraps to 0, `timeout` is still a large number,
so `running()` returns true for the entire next 49-day cycle — debounce
and workflow timers stay permanently "running."

**Current code:**
- `Timer` in `src/util/Timer.h`:
  ```cpp
  void start() { timeout = millis() + interval; }
  bool running() { return millis() < timeout; }
  ```
- Used by: Workflow (100ms debounce), Button (200ms), HAToggle (300ms),
  RotationToggle (200ms), TabBar (200ms).

**Fix:**
- Store start time instead of absolute timeout. Use unsigned subtraction
  (which handles overflow correctly for uint32_t):
  ```cpp
  unsigned long startTime = 0;
  bool started = false;
  void start() { startTime = millis(); started = true; }
  bool running() { return started && (millis() - startTime) < interval; }
  ```
- `(millis() - startTime)` wraps correctly because unsigned subtraction
  modulo 2^32 gives the correct elapsed time even across overflow.

**Estimated scope:** 3-line fix in Timer.h.

---

### P3. WiFi Reconnect

**Problem:** If WiFi drops after boot, the device shows "unavailable" for all
HA entities forever. No reconnect attempt, no user notification.

**Current code:**
- `ArduinoNetwork::init()` connects to WiFi with a 10-second timeout. If it
  fails, `_connected = false` and that's permanent.
- `Application::init()` checks `isConnected()` once at boot to decide whether
  to create the HA and OTA services.
- No periodic connectivity check in `Application::loop()`.

**Approach:**
- Add a `reconnect()` method to `INetwork` (default no-op for simulator).
- In `ArduinoNetwork::reconnect()`: if `WiFi.status() != WL_CONNECTED`,
  call `WiFi.reconnect()` with exponential backoff (1s, 2s, 4s, ... capped
  at 60s).
- In `Application::loop()`: periodically check
  `device()->network().isConnected()`. On disconnect, show a Toast
  ("WiFi disconnected") and start reconnect attempts. On reconnect, dismiss
  Toast and re-initialize HA/OTA if they were null.
- Check frequency: every 5-10 seconds (use a Timer).
- Don't block the main loop — just call `WiFi.reconnect()` and check status
  on the next cycle.

**Estimated scope:** ~40 lines across ArduinoNetwork + Application.

---

### P4. Heap Monitoring

**Problem:** On a device running 24/7, memory leaks or fragmentation can
silently degrade performance until the device crashes. No visibility into
heap state.

**Current code:**
- No heap monitoring anywhere. Arduino `String` concatenation in
  `HomeAssistant.cpp` does heap allocation on every HTTP call
  (`String(_baseUrl) + "/api/states/" + entityId`).
- Component trees are `new`'d and `delete`'d on screen transitions — potential
  for fragmentation over time.

**Approach:**
- In `Application::loop()`, periodically (every 30s) log:
  ```cpp
  Serial.printf("[HEAP] free=%d min=%d\n",
      esp_get_free_heap_size(),
      esp_get_minimum_free_heap_size());
  ```
- Set a critical threshold (~20KB). If free heap drops below it, log an error.
  Optionally: graceful reboot via `ESP.restart()` (with OTA rollback
  protection from P0).
- Later: feed into MetricsOverlay (#8) for on-screen display.
- Simulator: stub with a fixed large value or use platform malloc stats.

**Estimated scope:** ~15 lines in Application::loop() + simulator stub.

---

### P5. String Allocation Hygiene

**Problem:** Arduino `String` does heap allocation on every concatenation.
`HomeAssistant.cpp` builds URLs with `String(_baseUrl) + "/api/states/" + entityId`
on every call. Over thousands of calls across weeks of uptime, this fragments
the heap.

**Current code:**
- `HomeAssistant::getEntityState()` and `getEntityAttribute()` each create
  temporary `String` objects for URL construction.
- `INetwork` interface returns `HttpResponse { int statusCode; String body; }`
  — the `body` is a heap-allocated Arduino `String`.
- `OTAUpdate` similarly builds URLs with `String` concatenation.

**Approach:**
- Replace URL construction with `snprintf` into a fixed stack buffer:
  ```cpp
  char url[256];
  snprintf(url, sizeof(url), "%s/api/states/%s", _baseUrl, entityId);
  ```
- For `HttpResponse::body`: consider `std::string` (same heap allocation but
  better allocator behavior) or keep `String` but document the tradeoff.
- Low priority for the response body (it's allocated once per request and
  freed immediately). High priority for URL construction (called repeatedly
  in tight loops during screen render).

**Estimated scope:** ~20 lines across HomeAssistant.cpp and OTAUpdate.cpp.

---

## High Priority

### 1. HA Entity Cache (TTL-based)

**Problem:** Every `HAToggle`, `HAWeather`, `HABinarySensor` makes a blocking
HTTP GET on render via `HomeAssistant::getEntityState()` /
`getEntityAttribute()`. A screen with 4 lights = 4 serial round-trips before
paint completes.

**Current code:**
- `HomeAssistant::getEntityState()` and `getEntityAttribute()` in
  `src/application/services/HomeAssistant.cpp` — each builds a URL, does
  `_network->get()`, parses JSON, returns a String.
- No caching, no dedup. Two components reading the same entity make two
  identical HTTP calls.

**Approach:**
- Add a `std::map<String, CachedEntity>` to `HomeAssistant` where
  `CachedEntity` holds the full JSON response + timestamp.
- On `getEntityState()`/`getEntityAttribute()`, check cache first. If entry
  exists and `millis() - timestamp < TTL_MS`, return from cache.
- Single cache entry per entity_id (the `/api/states/{id}` response contains
  both state and attributes).
- TTL of 5-10 seconds is reasonable — HA state doesn't change that fast for
  most entities.
- `toggle()`/`turnOn()`/`turnOff()` should invalidate the cache entry for that
  entity so the next read fetches fresh state.
- Keep it simple — no background refresh, no LRU eviction. The cache is small
  (handful of entities) and TTL handles staleness.

**Estimated scope:** ~50 lines in `HomeAssistant.h/.cpp`.

---

### 2. Async / Non-blocking Network

**Problem:** `INetwork::get()` and `post()` are synchronous blocking calls.
The UI freezes during HTTP requests — LVGL can't render, touch events queue up,
and the 20ms `delay()` in `Application::loop()` doesn't help because the HTTP
call itself blocks for 100-500ms+.

**Current code:**
- `INetwork` interface in `src/device/INetwork.h` — `get()` and `post()` both
  return `HttpResponse` synchronously.
- `ArduinoNetwork` in `src/device/hw/drivers/network/ArduinoNetwork.cpp`
  implements via `HTTPClient`.
- `Application::loop()` calls `device()->touchscreen().pollEvent()` then
  `interface().loop()` then `delay(20)`.

**Approach:**
- Create a FreeRTOS task for network requests. Components post requests to a
  queue, the network task processes them and posts results back.
- Define a callback-based API alongside the sync one:
  ```cpp
  typedef void (*NetworkCallback)(HttpResponse response, void *userData);
  virtual void getAsync(const char *url, NetworkCallback cb,
                        void *userData, const char *authHeader = nullptr) = 0;
  ```
- The network task runs `HTTPClient` calls and invokes callbacks on the main
  thread (via a response queue polled in `Application::loop()`).
- Components switch from blocking fetch in `createWidgets` to: render
  "Loading..." state immediately, fire async request, update in callback.
- `StatefulComponent::update()` already exists for in-place widget updates —
  this is exactly the pattern needed.
- Simulator's `CurlNetwork` can implement the same interface (curl_multi or
  just a std::thread).

**Estimated scope:** Medium-large. New FreeRTOS task, queue plumbing, callback
types, update all HA components to async pattern.

**Dependencies:** Pairs well with HA Entity Cache (#1) — cache reduces the
number of async requests needed.

---

### 3. Navigation Back Stack

**Problem:** `Workflow::navigate()` is stateless — it stores `prevState` but
only one level deep. The "Back" button on the Details screen hardcodes its
return state. There's no generic way to "go back."

**Current code:**
- `Workflow` in `src/application/workflow/Workflow.h` — has `state` and
  `prevState`, both `State` enums. `navigate()` sets `prevState = state`
  before updating.
- `NavButton` navigates to a hardcoded `State` target.
- No history, no stack, no `goBack()` method.

**Approach:**
- Replace `prevState` with `std::vector<State> history` (or a small
  fixed-size ring buffer to avoid heap allocation).
- Add `Workflow::goBack()` — pops the last state from history and navigates
  to it. If history is empty, no-op or go to READY.
- `navigate()` pushes current state onto history before transitioning.
- Cap history depth (8-16 entries is plenty) to bound memory.
- Add `NavButton::back()` convenience constructor that calls
  `app->workflow().goBack()` instead of navigating to a fixed state.
- `onSwipeDown(ECOMODE)` in MainLayout could push to history so swiping
  back up from ECOMODE returns to the correct tab.

**Estimated scope:** ~30 lines in Workflow, small NavButton addition.

---

### 4. Error Recovery UI

**Problem:** Network errors silently show "unavailable" text. There's no retry
mechanism and no way for the user to recover without rebooting.

**Current code:**
- `HAToggle` shows entity_id + "Loading..." then "unavailable" on failure.
- `HAWeather` / `HABinarySensor` similar pattern.
- `HomeAssistant` service logs to Serial but returns default values.
- `Toast` exists for overlay notifications but isn't used for errors.

**Approach:**
- Add a retry callback pattern to `StatefulComponent`: on error, show a
  "Tap to retry" message and re-trigger the fetch on next tap.
- Or: use `Toast::show()` with an action button for transient network errors
  (e.g., "Connection lost — Retry").
- For persistent offline state: detect in `Application::loop()` when
  `device()->network().isConnected()` flips to false, show a persistent Toast
  or navigate to a dedicated offline screen.
- Consider a simple `ErrorBoundary` wrapper component that catches errors from
  children and shows a fallback UI.

**Estimated scope:** Small per-component, medium if building a generic pattern.

---

## Medium Priority

### 5. WebSocket for HA Real-time Updates

**Problem:** Polling HA via REST is inefficient and doesn't scale. Each entity
fetch is a full HTTP request/response cycle.

**Current code:**
- All HA interaction is REST-based via `INetwork::get()`/`post()`.
- Components use `lv_timer_create()` for periodic refresh (e.g., every 30s).

**Approach:**
- HA supports a WebSocket API at `ws://<host>:8123/api/websocket`.
- Authenticate once, then subscribe to `state_changed` events for specific
  entities.
- On state change, HA pushes the new state — no polling needed.
- Would require a WebSocket client library (e.g., `arduinoWebSockets` or
  ESP-IDF's built-in WebSocket client).
- Add an `IWebSocket` interface or extend `INetwork`.
- Components register interest in entity_ids; the WS service dispatches
  updates to registered callbacks.
- Pairs perfectly with async networking (#2) — the WS connection runs on the
  network task.
- Simulator shim could use libwebsockets or a simple C++ WS library.

**Estimated scope:** Large. New protocol, new service, component subscription
model.

---

### 6. State Enter/Exit Hooks

**Problem:** No way to run code when entering or leaving a screen. Components
hack around this with `lv_timer_create()` in `createWidgets()` for deferred
data fetching.

**Current code:**
- `ComponentManager::createComponent()` calls `createWidgets(screen)` then
  `lv_screen_load(screen)`. No lifecycle hooks.
- `Workflow::navigate()` fires a `WorkflowEvent` with `from` and `to` states.
- `Interface::handleEvent(WorkflowEvent)` triggers `createComponent(to)`.

**Approach:**
- Add `onEnter()` and `onExit()` virtual methods to `Component`.
- `ComponentManager::createComponent()` calls `active->onEnter()` after
  `createWidgets()` and `lv_screen_load()`.
- `ComponentManager::deleteComponent()` calls `active->onExit()` before
  `delete active`.
- Components use `onEnter()` to start data fetches, timers, animations.
- Components use `onExit()` to cancel pending requests, stop timers.
- Default implementations are no-ops (like `handleEvent`).

**Estimated scope:** ~10 lines in Component + ComponentManager. Each component
can adopt incrementally.

---

### 7. Slider Component

**Problem:** No way to control continuous values (light brightness, thermostat
setpoint, volume). LVGL has `lv_slider` built in but it's not wrapped.

**Approach:**
- `SliderProps { int min, max, value; uint32_t bg, indicator; }`.
- `SliderCallback` typedef like `ButtonCallback`.
- Wrap `lv_slider_create()`, style it, wire `LV_EVENT_VALUE_CHANGED` to the
  callback.
- HA integration: `HASlider` component that calls
  `ha->callService("light", "turn_on", entity, {{"brightness", value}})`.
- Hit-test / debounce: LVGL handles slider drag natively, but the HA call
  on value change should be debounced (user drags → only send final value
  after 300ms idle).

**Estimated scope:** ~80 lines for Slider, ~40 more for HASlider.

---

### 8. FPS / Memory Metrics Overlay

**Problem:** No visibility into runtime performance. `main.cpp:16` has a
`TODO render metrics` comment.

**Current code:**
- `Application::loop()` calls `app.loop()` with `delay(20)`.
- LVGL provides `lv_timer_get_idle()` (% of time LVGL is idle).
- ESP-IDF provides `esp_get_free_heap_size()`, `esp_get_minimum_free_heap_size()`.

**Approach:**
- Add a `MetricsOverlay` component that renders on `lv_layer_top()` (like
  Toast) showing FPS, free heap, LVGL idle %.
- Toggle via a setting or long-press gesture.
- FPS: count `lv_timer_handler()` calls per second in `Application::loop()`.
- Serial output alternative: log metrics every N seconds without UI overhead.
- Useful for catching regressions (e.g., adding a new component that tanks
  frame rate or leaks memory).

**Estimated scope:** ~60 lines.

---

### 9. Unit Tests (Host-side)

**Problem:** Zero tests. Regressions are caught by "flash and see if it
crashes."

**Current code:**
- Simulator build already compiles on host (CMake + SDL2).
- Core logic (Workflow, Timer, EventQueue, hit-testing) is pure C++ with no
  hardware dependencies.

**Approach:**
- Add a `test/` directory with a CMake target that links against the core
  source files (no LVGL, no device drivers).
- Use a lightweight test framework (Catch2 single-header, or doctest).
- Priority test targets:
  - `Workflow`: state transitions, debounce, `canNavigate()` rules, back stack.
  - `Timer`: start/running/stopped timing behavior.
  - `EventQueue`: subscribe/unsubscribe/post delivery.
  - `Button::hitTest()`: boundary conditions.
- Mock `millis()` for deterministic Timer tests.
- Can run in CI (GitHub Actions, no hardware needed).

**Estimated scope:** Test harness setup ~100 lines, individual tests ~20 lines
each.

---

### 10. Waveshare SD Card Support

**Problem:** `NoStorage` placeholder — Waveshare SD card CS is on CH422G EXIO4,
which needs the I/O expander to toggle before SPI transactions.

**Current code:**
- `NoStorage` in device drivers returns `hasError() = true`.
- `CH422G` driver manages pin state via shadow register, writes all 8 bits
  per update.
- EXIO4 = IO4 = SD_CS in the CH422G pin mapping.

**Approach:**
- Create `CH422GSDCard` storage driver that wraps `SD.begin()` with custom
  CS pin management through the CH422G.
- Before each SPI transaction: set EXIO4 LOW via CH422G shadow register.
- After transaction: set EXIO4 HIGH.
- May need to wrap `SD` library with a custom `SPIClass` that hooks
  `beginTransaction`/`endTransaction`.
- Alternative: use ESP-IDF's `sdmmc` driver directly with a custom GPIO
  interface.

**Estimated scope:** Medium. Requires careful SPI bus sharing between SD and
CH422G I2C.

---

## Low Priority / Polish

### 11. Arc / Chart Components

Wrap LVGL's `lv_arc` and `lv_chart` for data visualization on the Gauges
and Tuning screens. `lv_arc` is great for the round display — circular gauges
for coolant temp, oil pressure, etc.

### 12. Transition Animations

`ComponentManager::createComponent()` calls `lv_screen_load()` with no
animation. LVGL supports `lv_screen_load_anim()` with fade, slide, etc.
Could derive animation direction from navigation (swipe left → slide left).

### 13. Long-press Gesture

Touch drivers already detect tap and swipe. Add a long-press detection
(hold > 500ms without movement) to `TouchEvent` types. Enables contextual
actions (hold a light → brightness slider, hold a tab → screen options).

### 14. ~~OTA Rollback~~ → Moved to P0 (Critical)

### 15. Structured Logging

Replace `Serial.printf("[HA] ...")` with a `LOG(level, tag, fmt, ...)`
macro. Compile-time level filtering (strip DEBUG in release). Tags for
subsystems (HA, OTA, TOUCH, WORKFLOW). Small flash savings from removing
debug strings.

### 16. Image Component

Either revive the dead `src/lib/jpeg/` code behind the component system
or use LVGL's built-in image decoder (`lv_image_create` + `lv_image_set_src`).
Useful for weather condition icons, car diagrams, custom graphics. Would need
storage (#10) or compiled-in assets (`LV_IMAGE_DECLARE`).

### 17. Text Input / Numeric Keypad

A modal numeric keypad overlay for entering values (WiFi config, IP addresses,
thresholds). LVGL has `lv_keyboard` and `lv_textarea` built in. Wrap them in
the component system with a callback for the entered value.

### 18. Per-screen File Split

`Screens.cpp` is currently ~180 lines with all screen definitions. As screens
grow, split into `HomeScreen.h`, `GaugesScreen.h`, `TuningScreen.h`, etc.
Each file exports a single `RenderableComponent FooState()` function.
`Screens.h` becomes a barrel file of includes.

### 19. State Transition Validation

`Workflow::canNavigate()` currently allows any→any (except NOT_STARTED).
Add an optional transition table or guard callbacks for screens that should
only be reachable from specific states (e.g., DETAILS only from LIGHTS).

### 20. Config Persistence (NVS)

ESP32 has a built-in NVS (Non-Volatile Storage) partition for key-value
pairs. Use it to persist user preferences (rotation state, last active tab,
WiFi credentials, HA token) across reboots without needing an SD card.

### 21. Credential Provisioning

WiFi credentials and HA tokens are compile-time constants in gitignored
headers. For production: need a runtime provisioning flow. Options:
- **BLE provisioning** — ESP-IDF has `wifi_provisioning` component. Phone app
  sends WiFi SSID/password over BLE, device stores in NVS (#20).
- **Captive portal** — Device starts as AP, serves a web form, user enters
  credentials from phone browser.
- **Touchscreen entry** — Use Text Input (#17) for on-device credential entry.
- Minimum viable: read from NVS first, fall back to hardcoded compile-time
  values. This decouples the provisioning UI (can be added later) from the
  storage mechanism.

### 22. HTTPS Certificate Verification

OTA update URL uses plain HTTP or unverified HTTPS. A MITM could serve
malicious firmware (HMAC protects integrity but not if the attacker also
controls the key). For production: pin the OTA server's certificate or use
ESP-IDF's `esp_tls` with a CA bundle.

### 23. I2C Bus Recovery

ESP32 I2C bus can lock up if a slave holds SDA low (known hardware issue).
The GT911 re-init-after-RGB-panel hack in `Device.cpp` hints at this.
Production I2C code should have bus recovery: toggle SCL 9 times to release
a stuck slave, then send STOP condition. ESP-IDF's `i2c_master` driver
has built-in bus recovery support.

---

## Architecture — Control Server (Conceptual)

Unified server for OTA updates, device monitoring, remote configuration,
and (eventually) Home Assistant integration. Runs on a dedicated LXC on the
same LAN as the ESP32 devices.

This section is a design reference, not a task list. Implementation is future
work.

### What Exists Today

`server/ota_server.py` — a minimal FastAPI app that:
- Serves firmware binaries from `.pio/build/{board}/firmware.bin`
- Provides a `/api/version` endpoint (returns version + HMAC + size)
- Provides a `/api/firmware` endpoint (streams the binary)
- Config via `server.toml` (firmware_dir, secret_key, version, port)
- No device registry, no telemetry, no config push

On the device side:
- `OTAUpdate` service checks `/api/version?board={BOARD_ID}`, compares
  semver, downloads binary, verifies HMAC-SHA256, flashes.
- `Version.h` defines `FIRMWARE_VERSION` ("0.0.0") and `BOARD_ID`.
- Device identifies itself by `BOARD_ID` only — no unique device ID.

### Target Architecture

```
                         LAN
  ┌──────────────┐       │        ┌──────────────────────┐
  │  ESP32       │──wss://──────→ │  LXC Control Server  │
  │  Device A    │       │        │                      │
  └──────────────┘       │        │  ┌─ WebSocket Hub ─┐ │
                         │        │  │  Device sessions │ │
  ┌──────────────┐       │        │  │  Auth + routing  │ │
  │  ESP32       │──wss://──────→ │  └─────────────────┘ │
  │  Device B    │       │        │                      │
  └──────────────┘       │        │  ┌─ REST API ──────┐ │
                         │        │  │  OTA endpoints   │ │
  ┌──────────────┐       │        │  │  Device registry │ │
  │  Browser     │──wss://──────→ │  │  Config CRUD     │ │
  │  Dashboard   │       │        │  │  Telemetry query │ │
  └──────────────┘       │        │  └─────────────────┘ │
                         │        │                      │
                         │        │  ┌─ DB ────────────┐ │
                         │        │  │  Devices table   │ │
                         │        │  │  Telemetry (TS)  │ │
                         │        │  │  Config store    │ │
                         │        │  │  OTA releases    │ │
                         │        │  └─────────────────┘ │
                         │        │                      │
                         │        │  ┌─ MQTT bridge ───┐ │
                         │        │  │  (optional)      │ │
                         │        │  │  HA Discovery    │ │
                         │        │  └─────────────────┘ │
                         │        └──────────────────────┘
```

**Why WebSocket (device → server) instead of MQTT:**
- Topology is simple: N devices → 1 server, 1 dashboard → 1 server.
  No need for pub/sub fanout that justifies a broker.
- One fewer process to run — no Mosquitto. The server IS the endpoint
  for both devices and the browser dashboard.
- Bidirectional on one connection: telemetry upstream, config + OTA
  notifications downstream.
- HA integration handled by the server bridging to MQTT (server is an
  MQTT client, not a broker). Devices don't need to know about MQTT.
- TLS via `wss://` — ESP32 connects outbound, no ports exposed on devices.

### Device-Side Protocol

Devices connect to `wss://server.local:8443/ws/device` on boot.

**Connection lifecycle:**
1. Device opens WebSocket, sends auth message:
   ```json
   {"type": "auth", "device_id": "abc123", "board": "makerfabs_round_128",
    "fw_version": "1.2.3", "token": "pre-shared-device-token"}
   ```
2. Server validates token, registers device in DB (or updates last_seen).
3. Server responds: `{"type": "auth_ok", "config": {...current config...}}`
4. Bidirectional message flow begins.

**Device → Server messages:**
- `{"type": "telemetry", "heap": 245000, "min_heap": 220000,
   "uptime_s": 86400, "rssi": -52, "fps": 48, "lvgl_idle_pct": 85}`
  — sent every 30s (configurable).
- `{"type": "log", "level": "warn", "tag": "HA",
   "msg": "GET state failed: -1"}` — structured log forwarding.
- `{"type": "event", "name": "ota_complete", "version": "1.2.4"}`
  — lifecycle events (boot, ota_start, ota_complete, error, etc.).

**Server → Device messages:**
- `{"type": "config_update", "config": {"brightness": 80,
   "refresh_interval_s": 30, "active_entities": ["light.living_room"]}}`
  — pushed config changes. Device applies and stores in NVS (#20).
- `{"type": "ota_available", "version": "1.2.4"}`
  — server notifies device of new firmware. Device decides when to
  download (could be immediate or deferred to user action).
- `{"type": "ping"}` / `{"type": "pong"}`
  — keepalive. Detect dead connections on both sides.

**On disconnect:** Device reconnects with exponential backoff (1s, 2s, 4s,
... 60s cap). Server marks device as offline after missing 3 pings.

**Device identity:** Currently devices only have `BOARD_ID`. Need a unique
`DEVICE_ID` — options:
- Derive from ESP32 MAC address (`ESP.getEfuseMac()`) — unique per chip,
  no provisioning needed.
- Store in NVS during first provisioning.
- MAC-derived is simplest and sufficient for a LAN deployment.

### Server Implementation

**Language choice:**

The existing OTA server is Python FastAPI. Options for the full server:

| | Python (FastAPI) | Go | Rust | TypeScript (Deno/Bun) |
|---|---|---|---|---|
| **Existing code** | Already have OTA server | Rewrite | Rewrite | Rewrite |
| **WebSocket** | `websockets` or Starlette built-in | `gorilla/websocket` or stdlib | `tokio-tungstenite` | Native WS in Deno/Bun |
| **Async** | asyncio (single-threaded) | goroutines (excellent) | tokio (excellent) | Event loop (good) |
| **DB** | SQLAlchemy / raw SQL | `sqlx` / `pgx` | `sqlx` | Prisma / Drizzle |
| **Dashboard** | Serve static files + Jinja | Serve static files + templ | Serve static + Askama | Full-stack (Next.js etc.) |
| **Deploy** | pip + uvicorn | Single binary | Single binary | Single binary (Deno compile) |
| **Prototyping speed** | Fastest | Medium | Slowest | Fast |

**Recommendation: Python (FastAPI) for now, consider Go later.**
- You already have a working FastAPI server. Extending it with WebSocket
  support, a device registry, and telemetry storage is straightforward.
- FastAPI's Starlette layer has native WebSocket support — same framework,
  same process.
- For a handful of devices on a LAN, Python's performance is more than
  adequate. Async FastAPI handles thousands of concurrent WebSockets.
- If the server grows complex enough to justify it, Go is the natural
  next step (single binary deploy, great concurrency, easy cross-compile).
- Rust is overkill for a device management server. TypeScript is viable
  but doesn't offer advantages over Python here.

**Database:**
- **Dev/prototype:** SQLite. Zero config, file-based, shipped with Python.
  Good enough for <100 devices and months of telemetry.
- **Production:** PostgreSQL. Plain Postgres with a timestamp index on
  the telemetry table handles the expected scale (handful of devices,
  telemetry every 30s) without extensions. Periodic retention cleanup
  via cron (`DELETE WHERE time < now() - interval '90 days'`). If
  scale ever demands it, TimescaleDB is a drop-in Postgres extension
  that adds automatic partitioning — no schema changes needed.
- **Schema sketch:**
  ```sql
  -- Device registry
  devices (
    device_id TEXT PRIMARY KEY,  -- MAC-derived
    board TEXT NOT NULL,
    fw_version TEXT,
    config JSONB DEFAULT '{}',
    last_seen TIMESTAMPTZ,
    created_at TIMESTAMPTZ DEFAULT now()
  )

  -- Time-series telemetry (indexed on time, partition later if needed)
  telemetry (
    time TIMESTAMPTZ NOT NULL,
    device_id TEXT REFERENCES devices,
    heap_free INT,
    heap_min INT,
    uptime_s BIGINT,
    rssi INT,
    fps INT,
    lvgl_idle_pct INT
  )

  -- Structured log archive
  device_logs (
    time TIMESTAMPTZ NOT NULL,
    device_id TEXT REFERENCES devices,
    level TEXT,
    tag TEXT,
    message TEXT
  )

  -- OTA release tracking
  releases (
    id SERIAL PRIMARY KEY,
    board TEXT NOT NULL,
    version TEXT NOT NULL,
    firmware_path TEXT,
    hmac TEXT,
    size INT,
    created_at TIMESTAMPTZ DEFAULT now(),
    UNIQUE(board, version)
  )
  ```

### Dashboard

Vite + React (or Vue) SPA. The server is a pure API — REST for CRUD,
WebSocket for real-time — and the frontend is a static build artifact
served from the same process or nginx.

- Vite for fast dev iteration (HMR).
- WebSocket hook/store on the client for real-time device state updates.
- Charting library (Recharts, uPlot, or Chart.js) for telemetry
  visualization.
- Component reuse across dashboard views (device cards, log viewers,
  config editors).
- Production build is static files — deploy alongside the API server or
  behind a reverse proxy.

Dashboard views:
- **Device list** — all registered devices, online/offline status, firmware
  version, last seen, heap watermark.
- **Device detail** — live telemetry graphs (heap, FPS, RSSI over time),
  recent logs (streamed in real-time), config editor, OTA controls.
- **OTA management** — upload firmware, select target board/devices, track
  rollout progress.
- **Fleet overview** — aggregate health (% online, % on latest firmware,
  heap trends across devices).

### Home Assistant Integration (Optional)

The server acts as an MQTT client to bridge device data into HA:
- On device connect/telemetry: publish to
  `homeassistant/sensor/round_touch_{device_id}/state` with HA MQTT
  Discovery config. Devices auto-appear as HA entities.
- Expose: online status, firmware version, heap, RSSI, uptime as HA sensors.
- HA automations can trigger on device events (e.g., "alert when any device
  goes offline" or "notify when heap drops below threshold").
- The devices themselves never touch MQTT — the server handles the bridge.
  This keeps the device firmware simple and the MQTT integration swappable.

### Security Model

All LAN-internal, no cloud exposure.

- **Transport:** `wss://` (TLS) for device↔server and browser↔server.
  Self-signed cert is fine for LAN (devices trust the server's cert via
  pinning or a pre-provisioned CA).
- **Device auth:** Pre-shared token per device (or per board type). Stored
  in NVS on the device. Server validates on WebSocket connect.
- **Dashboard auth:** Session-based (cookie) or JWT. Since it's LAN-only,
  even basic auth is acceptable for a personal deployment.
- **OTA integrity:** Existing HMAC-SHA256 verification stays. Server signs
  firmware with the pre-shared key, device verifies before flashing.
- **Config push:** Server signs config payloads (HMAC or JWT) so devices
  can verify authenticity. Prevents a compromised network from injecting
  malicious config.

### Foundation Work (What to Build Now)

These items prepare the device firmware for the control server without
building the server itself:

1. **Unique device identity** — Derive `DEVICE_ID` from
   `ESP.getEfuseMac()`. Store in a global accessible from Application.
   Used for: server auth, telemetry tagging, MQTT topics.
   Scope: ~10 lines in `Version.h` or a new `DeviceInfo.h`.

2. **Structured logging (#15)** — `LOG(level, tag, fmt, ...)` macro with
   a pluggable backend. Today: writes to Serial. Tomorrow: also sends
   over WebSocket. The macro call sites don't change.
   Scope: ~30 lines for the macro + Serial backend.

3. **Telemetry collector** — A `Telemetry` struct/service that samples
   heap, uptime, RSSI, FPS on a timer. Today: logs to Serial (or feeds
   MetricsOverlay #8). Tomorrow: serializes to JSON and sends over
   WebSocket.
   Scope: ~40 lines.

4. **Config persistence via NVS (#20)** — Read/write key-value pairs.
   Today: stores rotation state, last tab. Tomorrow: stores server URL,
   device token, pushed config from server.
   Scope: ~50 lines for an NVS wrapper.

5. **WebSocket client stub** — Add `esp_websocket_client` (ESP-IDF
   component, already available) or `arduinoWebSockets` library. Create
   an `IControlChannel` interface with `connect()`, `send()`,
   `onMessage()`. Today: no-op or connects and sends telemetry.
   Scope: ~100 lines for interface + ESP-IDF implementation.

Items 1-4 are independently useful (improve logging, metrics, persistence)
and lay groundwork for the server integration. Item 5 is the actual
connection and can wait until the server exists.
