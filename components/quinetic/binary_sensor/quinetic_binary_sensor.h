#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/quinetic/quinetic.h"
#include "esphome/core/component.h"

#include <vector>

namespace esphome::quinetic {

class QuineticBinarySensor : public Component, public binary_sensor::BinarySensor, public DeviceItem {
 public:
  explicit QuineticBinarySensor(uint32_t address) : DeviceItem(address) {}

  void handle(const Message *msg) override;

  void dump_config() override;
};

}  // namespace esphome::quinetic
