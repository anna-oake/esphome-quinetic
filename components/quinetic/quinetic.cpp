#include "quinetic.h"
#include "esphome/core/log.h"
#include "protocol.h"

namespace esphome::quinetic {

static const char *const TAG = "quinetic";

void Quinetic::setup() {
  if (this->radio_ == nullptr) {
    ESP_LOGE(TAG, "No CC1101 configured");
    this->mark_failed();
    return;
  }
}

void Quinetic::dump_config() {
  ESP_LOGCONFIG(TAG, "Quinetic");
  LOG_UPDATE_INTERVAL(this);
}

void Quinetic::update() {
  poll_queue_.clear();
  poll_queue_.reserve(this->devices_.size());

  for (auto *d : this->devices_) {
    if (d->device_type == DeviceType::RELAY || d->device_type == DeviceType::SWITCH_RELAY)
      poll_queue_.push_back(d);
  }

  poll_index_ = 0;
  poll_repeat_ = 0;
  poll_next_();
}

void Quinetic::poll_next_() {
  if (poll_index_ >= poll_queue_.size())
    return;

  auto *d = poll_queue_[poll_index_];

  Message msg{
      .device_address = d->address,
      .device_type = d->device_type,
      .msg_type = MessageType::POLL,
      .msg_value = MessageValue::POLL,
  };

  this->transmit(&msg);

  poll_repeat_++;
  if (poll_repeat_ >= 2) {
    poll_repeat_ = 0;
    poll_index_++;
  }

  // space out the polls at 100ms to not choke the radio
  this->set_timeout("poll_next", 100, [this]() { this->poll_next_(); });
}

void Quinetic::on_packet(const std::vector<uint8_t> &raw, float freq_offset, float rssi, uint8_t lqi) {
  ESP_LOGV(TAG, "RX raw=%s freq=%.1f rssi=%.1f lqi=%u", format_hex_pretty(raw).c_str(), freq_offset, rssi, lqi);

  if (raw.size() != 12)
    return;

  std::vector<uint8_t> p;
  p.reserve(10);
  for (size_t i = 0; i < 10; i++) {
    uint8_t b = (uint8_t) ((raw[i + 1] << 1) | (raw[i + 2] >> 7));
    p.push_back(b);
  }

  ESP_LOGV(TAG, "RX unshifted=%s", format_hex_pretty(p).c_str());

  Message msg;
  if (!decode_packet_(p, msg)) {
    ESP_LOGV(TAG, "RX: Invalid CRC");
    return;
  }

  this->on_receive_(&msg, rssi, lqi);
}

void Quinetic::on_receive_(const Message *msg, float rssi, uint8_t lqi) {
  auto device = find_device_(msg->device_address);
  if (!device) {
    ESP_LOGW(TAG, "RX: device=%08X type=%s msg=%s val=%s", msg->device_address, to_string(msg->device_type),
             to_string(msg->msg_type), to_string(msg->msg_value));
    return;
  };
  ESP_LOGD(TAG, "RX: device=%08X type=%s msg=%s val=%s", msg->device_address, to_string(msg->device_type),
           to_string(msg->msg_type), to_string(msg->msg_value));
  device->handle(msg);
}

void Quinetic::transmit(const Message *msg) {
  if (msg->device_type == DeviceType::SWITCH) {
    ESP_LOGE(TAG, "TX for switches is not implemented");
    return;
  }

  ESP_LOGD(TAG, "TX: device=%08X type=%s msg=%s val=%s", msg->device_address, to_string(msg->device_type),
           to_string(msg->msg_type), to_string(msg->msg_value));

  std::vector<uint8_t> p;
  encode_packet_(msg, p);

  ESP_LOGV(TAG, "TX unshifted=%s", format_hex_pretty(p).c_str());

  std::vector<uint8_t> raw(12);
  raw[0] = 0x11;

  for (size_t i = 0; i < 10; i++) {
    const uint8_t prev_lsb = (i == 0) ? 1 : (p[i - 1] & 0x01);
    raw[i + 1] = (uint8_t) ((p[i] >> 1) | (prev_lsb << 7));
  }

  raw[11] = (uint8_t) (p[9] << 7);

  ESP_LOGV(TAG, "TX raw=%s", format_hex_pretty(raw).c_str());

  this->radio_->transmit_packet(raw);
}

}  // namespace esphome::quinetic
