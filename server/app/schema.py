"""Component schema — mirrors src/ui/registry/ComponentFactories.h.

Served to the web editor so it can dynamically build prop forms.
"""

COMPONENT_SCHEMA = {
    "Text": {
        "container": False,
        "fields": {
            "text": {"type": "string", "default": "", "label": "Text content", "location": "top"},
        },
        "props": {
            "size": {"type": "int", "default": 3, "min": 1, "max": 5, "label": "Font size"},
            "color": {"type": "color", "default": None, "label": "Text color"},
        },
    },
    "Card": {
        "container": True,
        "fields": {},
        "props": {
            "bg": {"type": "color", "default": None, "label": "Background"},
            "border": {"type": "color", "default": None, "label": "Border"},
            "radius": {"type": "int", "default": None, "label": "Radius"},
            "pad": {"type": "int", "default": None, "label": "Padding"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
        },
    },
    "FillScreen": {
        "container": True,
        "fields": {},
        "props": {
            "color": {"type": "color", "default": None, "label": "Background"},
            "pad": {"type": "int", "default": None, "label": "Padding"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
        },
    },
    "FlexLayout": {
        "container": True,
        "fields": {},
        "props": {
            "direction": {"type": "enum", "options": ["row", "column"], "default": "column", "label": "Direction"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
            "align": {"type": "enum", "options": ["left", "center", "right"], "default": "left", "label": "Align"},
        },
    },
    "ScrollContainer": {
        "container": True,
        "fields": {},
        "props": {
            "pad": {"type": "int", "default": None, "label": "Padding"},
            "gap": {"type": "int", "default": None, "label": "Gap"},
            "maxWidth": {"type": "int", "default": None, "label": "Max width"},
        },
    },
    "TitledCard": {
        "container": True,
        "fields": {},
        "props": {
            "icon": {"type": "icon", "default": "", "label": "Icon"},
            "title": {"type": "string", "default": "", "label": "Title"},
            "bg": {"type": "color", "default": None, "label": "Background"},
            "border": {"type": "color", "default": None, "label": "Border"},
        },
    },
    "GaugeCard": {
        "container": False,
        "fields": {},
        "props": {
            "label": {"type": "string", "default": "", "label": "Label"},
            "value": {"type": "string", "default": "", "label": "Value"},
        },
    },
    "HAToggle": {
        "container": False,
        "props": {},
        "fields": {
            "entity": {"type": "string", "default": "", "label": "HA entity ID", "location": "top"},
        },
    },
    "HAWeather": {
        "container": False,
        "props": {},
        "fields": {
            "entity": {"type": "string", "default": "", "label": "HA entity ID", "location": "top"},
        },
    },
    "HABinarySensor": {
        "container": False,
        "props": {},
        "fields": {
            "entity": {"type": "string", "default": "", "label": "HA entity ID", "location": "top"},
            "label": {"type": "string", "default": "", "label": "Display label", "location": "top"},
        },
    },
    "DynamicText": {
        "container": False,
        "fields": {
            "template": {"type": "string", "default": "", "label": "Template (use {{source:key}} for data)", "location": "top"},
            "entities": {"type": "string", "default": "", "label": "Data sources (comma-separated)", "location": "top"},
        },
        "props": {
            "ttl": {"type": "int", "default": 60, "min": 10, "max": 86400, "label": "Refresh interval (seconds)"},
            "size": {"type": "int", "default": 3, "min": 1, "max": 5, "label": "Font size"},
            "color": {"type": "color", "default": None, "label": "Text color"},
        },
    },
    "LLMText": {
        "container": False,
        "fields": {
            "prompt": {"type": "string", "default": "", "label": "LLM prompt (use {{source:key}} for context)", "location": "top"},
            "model": {"type": "enum", "options": ["qwen3-8b", "qwen3-30b"], "default": "qwen3-8b", "label": "LLM model"},
            "entities": {"type": "string", "default": "", "label": "Data sources (comma-separated)", "location": "top"},
        },
        "props": {
            "ttl": {"type": "int", "default": 300, "min": 30, "max": 86400, "label": "Cache TTL (seconds)"},
            "size": {"type": "int", "default": 3, "min": 1, "max": 5, "label": "Font size"},
            "color": {"type": "color", "default": None, "label": "Text color"},
        },
    },
}

KNOWN_COMPONENT_TYPES = set(COMPONENT_SCHEMA.keys())
