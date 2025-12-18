#include "quinetic_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome::quinetic {

static const char *const TAG = "quinetic.binary_sensor";

void QuineticBinarySensor::dump_config() { LOG_BINARY_SENSOR("", "Quinetic Binary Sensor", this); }

void QuineticBinarySensor::handle(const Message *msg) { this->publish_state(msg->msg_value == MessageValue::PRESS); }

}  // namespace esphome::quinetic
