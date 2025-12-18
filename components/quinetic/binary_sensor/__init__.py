import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ADDRESS, CONF_ID

from .. import CONF_QUINETIC_ID, DeviceItem, DeviceItemBaseSchema, quinetic_ns

DEPENDENCIES = ["quinetic"]
CODEOWNERS = ["@anna-oake"]

QuineticBinarySensor = quinetic_ns.class_(
    "QuineticBinarySensor", cg.Component, binary_sensor.BinarySensor, DeviceItem
)

CONFIG_SCHEMA = cv.All(
    binary_sensor.binary_sensor_schema(QuineticBinarySensor)
    .extend(cv.COMPONENT_SCHEMA)
    .extend(DeviceItemBaseSchema)
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_ADDRESS],
    )
    await cg.register_component(var, config)
    await binary_sensor.register_binary_sensor(var, config)

    paren = await cg.get_variable(config[CONF_QUINETIC_ID])
    cg.add(paren.add_device_item(var))
