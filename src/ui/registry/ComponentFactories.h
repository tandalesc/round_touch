#ifndef _COMPONENT_FACTORIES_H_
#define _COMPONENT_FACTORIES_H_

#include "ui/registry/ComponentRegistry.h"

// Components
#include "application/interface/components/core/Text.h"
#include "application/interface/components/core/Card.h"
#include "application/interface/components/core/FillScreen.h"
#include "application/interface/components/layout/FlexLayout.h"
#include "application/interface/components/input/ScrollContainer.h"
#include "application/interface/components/ha/HAToggle.h"
#include "application/interface/components/ha/HAWeather.h"
#include "application/interface/components/ha/HABinarySensor.h"
#include "config/screens/GaugeCard.h"
#include "config/screens/TitledCard.h"

// ---------------------------------------------------------------------------
// Factory functions — one per registerable component
// Each receives the full JSON node and pre-built children vector.
// Props are read from node["props"], top-level keys like "text"/"entity"
// are read directly from node.
// ---------------------------------------------------------------------------

static Component *createText(const JsonObject &node,
                              std::vector<Component *> children) {
  TextProps p;
  JsonObject props = node["props"];
  if (props) {
    if (props["size"])  p.size  = props["size"].as<uint8_t>();
    if (props["color"]) p.color = props["color"].as<uint32_t>();
  }
  const char *text = node["text"] | "";
  return new Text(p, text);
}

static Component *createCard(const JsonObject &node,
                              std::vector<Component *> children) {
  CardProps p;
  JsonObject props = node["props"];
  if (props) {
    if (props["bg"])     p.bg     = props["bg"].as<uint32_t>();
    if (props["border"]) p.border = props["border"].as<uint32_t>();
    if (props["radius"]) p.radius = props["radius"].as<int>();
    if (props["pad"])    p.pad    = props["pad"].as<int>();
    if (props["gap"])    p.gap    = props["gap"].as<int>();
  }
  return new Card(p, std::vector<RenderableComponent>(children.begin(),
                                                       children.end()));
}

static Component *createFillScreen(const JsonObject &node,
                                    std::vector<Component *> children) {
  FillScreenProps p;
  JsonObject props = node["props"];
  if (props) {
    if (props["color"]) p.color = props["color"].as<uint32_t>();
    if (props["pad"])   p.pad   = props["pad"].as<int>();
    if (props["gap"])   p.gap   = props["gap"].as<int>();
  }
  return new FillScreen(p, std::vector<RenderableComponent>(children.begin(),
                                                             children.end()));
}

static Component *createFlexLayout(const JsonObject &node,
                                    std::vector<Component *> children) {
  LayoutContext ctx;
  JsonObject props = node["props"];
  if (props) {
    const char *dir = props["direction"] | "column";
    if (strcmp(dir, "row") == 0)
      ctx.type = LayoutType::Row;
    else
      ctx.type = LayoutType::Column;

    if (props["gap"]) ctx.props.gap = props["gap"].as<int>();

    const char *align = props["align"] | "left";
    if (strcmp(align, "center") == 0)
      ctx.align = Align::Center;
    else if (strcmp(align, "right") == 0)
      ctx.align = Align::Right;
    else
      ctx.align = Align::Left;
  }
  return new FlexLayout(ctx, std::vector<RenderableComponent>(children.begin(),
                                                               children.end()));
}

static Component *createScrollContainer(const JsonObject &node,
                                         std::vector<Component *> children) {
  ScrollContainerProps p;
  JsonObject props = node["props"];
  if (props) {
    if (props["pad"])      p.pad      = props["pad"].as<int>();
    if (props["gap"])      p.gap      = props["gap"].as<int>();
    if (props["maxWidth"]) p.maxWidth = props["maxWidth"].as<int>();
  }
  return new ScrollContainer(p, std::vector<RenderableComponent>(
                                    children.begin(), children.end()));
}

static Component *createGaugeCard(const JsonObject &node,
                                   std::vector<Component *> children) {
  GaugeCardProps p;
  JsonObject props = node["props"];
  if (props) {
    if (props["label"]) p.label = props["label"].as<const char *>();
    if (props["value"]) p.value = props["value"].as<const char *>();
  }
  return new GaugeCard(p);
}

static Component *createTitledCard(const JsonObject &node,
                                    std::vector<Component *> children) {
  TitledCardProps p;
  JsonObject props = node["props"];
  if (props) {
    if (props["icon"])   p.icon   = props["icon"].as<const char *>();
    if (props["title"])  p.title  = props["title"].as<const char *>();
    if (props["bg"])     p.bg     = props["bg"].as<uint32_t>();
    if (props["border"]) p.border = props["border"].as<uint32_t>();
  }
  return new TitledCard(p, std::vector<RenderableComponent>(children.begin(),
                                                             children.end()));
}

static Component *createHAToggle(const JsonObject &node,
                                  std::vector<Component *> children) {
  const char *entity = node["entity"] | "";
  return new HAToggle(entity);
}

static Component *createHAWeather(const JsonObject &node,
                                   std::vector<Component *> children) {
  const char *entity = node["entity"] | "";
  return new HAWeather(entity);
}

static Component *createHABinarySensor(const JsonObject &node,
                                        std::vector<Component *> children) {
  const char *entity = node["entity"] | "";
  const char *label  = node["label"]  | "";
  return new HABinarySensor(entity, label);
}

// ---------------------------------------------------------------------------
// Registration — call once at boot
// ---------------------------------------------------------------------------

static void registerAllComponents(ComponentRegistry &registry) {
  // Tier 1 — primitives
  registry.reg("Text",            createText);
  registry.reg("Card",            createCard);
  registry.reg("FillScreen",      createFillScreen);
  registry.reg("FlexLayout",      createFlexLayout);
  registry.reg("ScrollContainer", createScrollContainer);
  registry.reg("GaugeCard",       createGaugeCard);
  registry.reg("TitledCard",      createTitledCard);
  // Tier 2 — provided components
  registry.reg("HAToggle",        createHAToggle);
  registry.reg("HAWeather",       createHAWeather);
  registry.reg("HABinarySensor",  createHABinarySensor);
}

#endif // _COMPONENT_FACTORIES_H_
