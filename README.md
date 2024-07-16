# Round Touch Experiment

Small UI experiment.

## Requirements

Device was developed surrounding a MakerFabs ESP32 device with
a built-in 1.28" TFT touch screen. The product's GitHub page as
well as links to MakerFabs's page is located [here][makerfabslink].
You can download the dependencies suggested on that page as well,
most of it is required for this project as well.

This library relies on Arduino binaries to compile and deploy its code.
Most of the library is completely device agnostic, besides some specific
interactions with the display and touch screen drivers. The rest can be
utilized with any compatible device assuming you make the necessary changes
to the device driver layer.

This library relies on recent C++ features to work properly. See included
`.vscode` configuration files to learn more.

## Architecture

### Overview

```text
Device
    Application
        Workflow
        Interface
            Components...
    Events...
    Device Drivers...
```

### Interface Components

More functionality is planned in the future.

```text
Component
    TouchNavigation
    Text
    ComponentWithChildren
        FillScreen
        FlexLayout
```

### Events

```text
EventQueue<Event>
    EventHandler<Event>
    Event
        TouchEvent
        WorkflowEvent
```

### Device Drivers

```text
Device
    Display
        Arduino_ESP32SPI
        Arduino_GC9A01
    SDCard
        SDFileSystemClass
    TouchScreen
        CST816S
```

[makerfabslink]: https://github.com/Makerfabs/ESP32-S3-Round-SPI-TFT-with-Touch-1.28/tree/main