#pragma once

#include "esphome/components/quinetic/quinetic.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include <vector>

namespace esphome::quinetic {

class QuineticSwitch : public Component, public switch_::Switch, public DeviceItem {
 public:
  explicit QuineticSwitch(uint32_t address) : DeviceItem(address) {}

  void handle(const Message *msg) override;
  void write_state(bool state) override;

  void dump_config() override;

  void set_parent(Quinetic *parent) { this->parent_ = parent; }

 protected:
  Quinetic *parent_{nullptr};
};

}  // namespace esphome::quinetic
