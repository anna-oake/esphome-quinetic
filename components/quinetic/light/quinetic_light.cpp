#include "quinetic_light.h"
#include "esphome/core/log.h"

namespace esphome::quinetic {

static const char *const TAG = "quinetic.light";

void QuineticLight::dump_config() {}

void QuineticLight::handle(const Message *msg) {
  if (msg->msg_value != MessageValue::ON && msg->msg_value != MessageValue::OFF) {
    return;
  }
  bool on = msg->msg_value == MessageValue::ON;

  bool cur;
  this->light_state_->current_values_as_binary(&cur);
  if (cur == on)
    return;

  this->light_state_->remote_values.set_state(on);
  this->light_state_->publish_state();
}

void QuineticLight::write_state(light::LightState *state) {
  bool binary;
  state->current_values_as_binary(&binary);
  Message msg = Message{.device_address = this->address,
                        .device_type = this->device_type,
                        .msg_type = MessageType::SET,
                        .msg_value = binary ? MessageValue::ON : MessageValue::OFF};
  this->parent_->transmit(&msg);
}

}  // namespace esphome::quinetic
