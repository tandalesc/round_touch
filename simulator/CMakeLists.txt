cmake_minimum_required(VERSION 3.16)
project(round_touch_sim C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Board define - must be set before LVGL so lv_conf.h can use it
add_compile_definitions(BOARD_SIMULATOR)

# Allow overriding screen dimensions: cmake .. -DSCREEN_WIDTH=800 -DSCREEN_HEIGHT=480
if(DEFINED SCREEN_WIDTH)
    add_compile_definitions(SCREEN_WIDTH=${SCREEN_WIDTH})
endif()
if(DEFINED SCREEN_HEIGHT)
    add_compile_definitions(SCREEN_HEIGHT=${SCREEN_HEIGHT})
endif()

# Find SDL2 and CURL
find_package(SDL2 REQUIRED)
find_package(CURL REQUIRED)

# ArduinoJson (header-only, used by HomeAssistant service)
include(FetchContent)
FetchContent_Declare(
    ArduinoJson
    GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
    GIT_TAG        v7.3.0
)
FetchContent_MakeAvailable(ArduinoJson)

# SDL2 parent include (LVGL needs <SDL2/SDL.h> which requires the parent dir)
get_filename_component(SDL2_INCLUDE_PARENT "${SDL2_INCLUDE_DIRS}" DIRECTORY)

# LVGL configuration
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib/lv_conf.h CACHE STRING "" FORCE)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
set(LV_CONF_BUILD_DISABLE_DEMOS ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/lvgl ${CMAKE_BINARY_DIR}/lvgl)

# LVGL needs SDL2 headers for its built-in SDL driver
target_include_directories(lvgl PUBLIC ${SDL2_INCLUDE_PARENT})

# Include paths - ORDER MATTERS
# 1. Shims directory (overrides Arduino library headers like <Arduino.h>)
# 2. Simulator directory (for platform/ includes)
# 3. src/ directory (include root for application code)
# 4. LVGL conf (next to lvgl/ folder)
# 5. SDL2 headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/shims
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
    ${SDL2_INCLUDE_DIRS}
)

# Simulator-specific sources
set(SIM_SOURCES
    main.cpp
    platform/SimTouch.cpp
    platform/CurlNetwork.cpp
)

# Shared application sources from src/ (compile against shims unchanged)
set(APP_SOURCES
    ../src/application/Application.cpp
    ../src/application/interface/Interface.cpp
    ../src/application/interface/components/ComponentManager.cpp
    ../src/application/interface/components/types/Component.cpp
    ../src/application/interface/components/types/ComponentWithChildren.cpp
    ../src/application/interface/components/types/Layout.cpp
    ../src/application/interface/components/layout/FlexLayout.cpp
    ../src/application/workflow/Workflow.cpp
    ../src/config/screens/Screens.cpp
    ../src/config/screens/Routes.cpp
    ../src/application/services/HomeAssistant.cpp
    ../src/application/services/OTAUpdate.cpp
    ../src/application/interface/Toast.cpp
    ../src/device/Device.cpp
)

add_executable(round_touch_sim ${SIM_SOURCES} ${APP_SOURCES})
target_link_libraries(round_touch_sim lvgl ${SDL2_LIBRARIES} CURL::libcurl ArduinoJson)
