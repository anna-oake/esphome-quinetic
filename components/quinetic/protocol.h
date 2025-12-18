#pragma once

#include <cstdint>
#include <vector>

namespace esphome::quinetic {

enum class DeviceType : uint8_t {
  UNKNOWN,
  SWITCH,
  RELAY,
  SWITCH_RELAY,
};

enum class MessageType : uint8_t {
  UNKNOWN,
  STATE,
  POLL,
  SET,
};

enum class MessageValue : uint8_t {
  UNKNOWN,
  POLL,
  PRESS,
  RELEASE,
  OFF,
  ON,
};

struct Message {
  uint32_t device_address{};
  DeviceType device_type{};
  MessageType msg_type{};
  MessageValue msg_value{};
};

inline constexpr const char *to_string(DeviceType t) {
  switch (t) {
    case DeviceType::UNKNOWN:
      return "UNKNOWN";
    case DeviceType::SWITCH:
      return "SWITCH";
    case DeviceType::RELAY:
      return "RELAY";
    case DeviceType::SWITCH_RELAY:
      return "SWITCH_RELAY";
  }
  return "UNKNOWN";
}

inline constexpr const char *to_string(MessageType t) {
  switch (t) {
    case MessageType::UNKNOWN:
      return "UNKNOWN";
    case MessageType::STATE:
      return "STATE";
    case MessageType::POLL:
      return "POLL";
    case MessageType::SET:
      return "SET";
  }
  return "UNKNOWN";
}

inline constexpr const char *to_string(MessageValue v) {
  switch (v) {
    case MessageValue::UNKNOWN:
      return "UNKNOWN";
    case MessageValue::POLL:
      return "POLL";
    case MessageValue::PRESS:
      return "PRESS";
    case MessageValue::RELEASE:
      return "RELEASE";
    case MessageValue::OFF:
      return "OFF";
    case MessageValue::ON:
      return "ON";
  }
  return "UNKNOWN";
}

void decode_small_packet_(const std::vector<uint8_t> &packet, Message &out);
void decode_big_packet_(const std::vector<uint8_t> &packet, Message &out);
bool decode_packet_(const std::vector<uint8_t> &packet, Message &out);

void encode_packet_(const Message *msg, std::vector<uint8_t> &out);

uint16_t crc16_(const uint8_t *buffer, int len);

}  // namespace esphome::quinetic
