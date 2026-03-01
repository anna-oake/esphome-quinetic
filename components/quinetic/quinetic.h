#pragma once

#include "esphome/components/cc1101/cc1101.h"
#include "esphome/core/component.h"
#include "protocol.h"

#include <vector>

namespace esphome::quinetic {

class DeviceItem {
 public:
  explicit DeviceItem(uint32_t address) : address(address), device_type(guess_device_type_(address)) {}
  virtual ~DeviceItem() = default;

  virtual void handle(const Message *msg) = 0;
  uint32_t address{0};
  DeviceType device_type{DeviceType::UNKNOWN};

 private:
  static DeviceType guess_device_type_(uint32_t address) {
    if (address <= 0xFFFF) {
      return DeviceType::SWITCH;
    }
    switch ((address >> 24) & 0xFF) {
      case 0xB1:
        return DeviceType::SWITCH_RELAY;
      case 0xB0:
      case 0x00:
        return DeviceType::RELAY;
    }
    return DeviceType::UNKNOWN;
  }
};

class Quinetic : public PollingComponent, public cc1101::CC1101Listener {
 public:
  void set_radio(cc1101::CC1101Component *radio) { radio_ = radio; }

  void setup() override;
  void dump_config() override;
  void update() override;
  
  void on_packet(const std::vector<uint8_t> &packet, float freq_offset, float rssi, uint8_t lqi) override;

  void add_device_item(DeviceItem *item) { devices_.push_back(item); }

  void transmit(const Message *msg);

 protected:
  void on_receive_raw_(const std::vector<uint8_t> &raw, float freq_offset, float rssi, uint8_t lqi);
  void on_receive_(const Message *msg, float rssi, uint8_t lqi);
  void poll_next_();

  cc1101::CC1101Component *radio_{nullptr};
  std::vector<DeviceItem *> devices_;

  std::vector<DeviceItem *> poll_queue_;
  size_t poll_index_{0};
  uint8_t poll_repeat_{0};

  DeviceItem *find_device_(uint32_t address) {
    for (auto d : devices_)
      if (d->address == address)
        return d;
    return nullptr;
  }
};

}  // namespace esphome::quinetic
