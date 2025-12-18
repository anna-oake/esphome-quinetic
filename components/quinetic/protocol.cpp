#include "protocol.h"

namespace esphome::quinetic {
void decode_small_packet_(const std::vector<uint8_t> &p, Message &out) {
  out.device_type = DeviceType::SWITCH;                // small packets only come from switches (?)
  out.device_address = ((uint32_t) p[0] << 8) | p[1];  // switches have 16-bit IDs
  out.msg_type = MessageType::STATE;                   // switches only send state messages
  switch (p[2]) {
    case 0x04:
      out.msg_value = MessageValue::PRESS;
      break;
    case 0xC0:
    case 0x08:
      out.msg_value = MessageValue::RELEASE;
      break;
    default:
      out.msg_value = MessageValue::UNKNOWN;
      break;
  }
}

void decode_big_packet_(const std::vector<uint8_t> &p, Message &out) {
  switch (p[0]) {
    case 0xB1:
      out.device_type = DeviceType::SWITCH_RELAY;
      break;
    case 0xB0:
    case 0x00:
      out.device_type = DeviceType::RELAY;
      break;
    default:
      out.device_type = DeviceType::UNKNOWN;
      break;
  }
  out.device_address = ((uint32_t) p[0] << 24) | ((uint32_t) p[1] << 16) | ((uint32_t) p[2] << 8) | p[3];
  switch (p[4]) {
    case 0x0D:
      out.msg_type = MessageType::POLL;
      out.msg_value = (p[7] == 0x01) ? MessageValue::ON : (p[7] == 0x00) ? MessageValue::OFF : MessageValue::UNKNOWN;
      break;
    case 0x0B:
      out.msg_type = MessageType::SET;
      out.msg_value = (p[7] == 0x01) ? MessageValue::ON : (p[7] == 0x00) ? MessageValue::OFF : MessageValue::UNKNOWN;
      break;
    case 0x0C:
    case 0xA5:
      out.msg_type = MessageType::STATE;
      out.msg_value = (p[7] == 0x01) ? MessageValue::ON : (p[7] == 0x00) ? MessageValue::OFF : MessageValue::UNKNOWN;
      break;
    default:
      out.msg_type = MessageType::UNKNOWN;
      out.msg_value = MessageValue::UNKNOWN;
      break;
  }
}

bool decode_packet_(const std::vector<uint8_t> &packet, Message &out) {
  bool small = false;

  uint16_t crc = crc16_(packet.data(), 3);
  if (crc == ((packet[3] << 8) | packet[4])) {
    decode_small_packet_(packet, out);
    return true;
  }

  crc = crc16_(packet.data(), 8);
  if (crc == ((packet[8] << 8) | packet[9])) {
    decode_big_packet_(packet, out);
    return true;
  }

  return false;
}

void encode_packet_(const Message *msg, std::vector<uint8_t> &out) {
  out.resize(10);

  out[0] = (uint8_t) ((msg->device_address >> 24) & 0xFF);
  out[1] = (uint8_t) ((msg->device_address >> 16) & 0xFF);
  out[2] = (uint8_t) ((msg->device_address >> 8) & 0xFF);
  out[3] = (uint8_t) ((msg->device_address >> 0) & 0xFF);

  const bool poll = (msg->msg_type == MessageType::POLL);

  out[4] = (uint8_t) (poll ? 0x0D : 0x0B);
  out[5] = (uint8_t) (poll ? 0x00 : 0x01);
  out[6] = (uint8_t) (poll ? 0x55 : (msg->device_type == DeviceType::SWITCH_RELAY) ? 0x07 : 0x02);
  out[7] = (uint8_t) (poll ? 0xAA : (msg->msg_value == MessageValue::ON) ? 0x01 : 0x00);

  const uint16_t crc = crc16_(out.data(), 8);
  out[8] = (uint8_t) ((crc >> 8) & 0xFF);
  out[9] = (uint8_t) (crc & 0xFF);
}

uint16_t crc16_(const uint8_t *buffer, int len) {
  uint16_t crc = 0x1D0F;  // AUG-CCITT init
  const uint16_t poly = 0x1021;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t) buffer[i] << 8;
    for (int b = 0; b < 8; b++) {
      if (crc & 0x8000)
        crc = (uint16_t) ((crc << 1) ^ poly);
      else
        crc = (uint16_t) (crc << 1);
    }
  }
  return crc;
}
}  // namespace esphome::quinetic
