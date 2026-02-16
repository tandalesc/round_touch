# Ideas & Improvements

Prioritized list of features and improvements for round_touch.
Each item includes context on what exists today and notes for implementation.

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

### 14. OTA Rollback

ESP-IDF supports `esp_ota_mark_app_valid_cancel_rollback()`. If the app
doesn't call this within N seconds of boot, the bootloader reverts to the
previous partition. Add the call to `Application::init()` after successful
startup, providing automatic brick protection.

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
