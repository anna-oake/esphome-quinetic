#pragma once

#include "esphome/components/light/light_output.h"
#include "esphome/components/quinetic/quinetic.h"
#include "esphome/core/component.h"

#include <vector>

namespace esphome::quinetic {

class QuineticLight : public Component, public light::LightOutput, public DeviceItem {
 public:
  explicit QuineticLight(uint32_t address) : DeviceItem(address) {}

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::ON_OFF});
    return traits;
  }

  void handle(const Message *msg) override;
  void setup_state(light::LightState *state) override { this->light_state_ = state; }
  void write_state(light::LightState *state) override;

  void dump_config() override;

  void set_parent(Quinetic *parent) { this->parent_ = parent; }

 protected:
  Quinetic *parent_{nullptr};
  light::LightState *light_state_{nullptr};
};

}  // namespace esphome::quinetic
