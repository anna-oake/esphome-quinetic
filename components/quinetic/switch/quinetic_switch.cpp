#include "quinetic_switch.h"
#include "esphome/core/log.h"

namespace esphome::quinetic {

static const char *const TAG = "quinetic.switch";

void QuineticSwitch::dump_config() { LOG_SWITCH("", "Quinetic Switch", this); }

void QuineticSwitch::handle(const Message *msg) {
  if (msg->msg_value != MessageValue::ON && msg->msg_value != MessageValue::OFF) {
    return;
  }
  this->publish_state(msg->msg_value == MessageValue::ON);
}

void QuineticSwitch::write_state(bool state) {
  Message msg = Message{.device_address = this->address,
                        .device_type = this->device_type,
                        .msg_type = MessageType::SET,
                        .msg_value = state ? MessageValue::ON : MessageValue::OFF};
  this->parent_->transmit(&msg);
}

}  // namespace esphome::quinetic
