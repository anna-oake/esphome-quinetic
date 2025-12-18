import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ADDRESS, CONF_OUTPUT_ID

from .. import CONF_QUINETIC_ID, DeviceItem, DeviceItemBaseSchema, quinetic_ns

DEPENDENCIES = ["quinetic"]
CODEOWNERS = ["@anna-oake"]

QuineticLight = quinetic_ns.class_(
    "QuineticLight", cg.Component, light.LightOutput, DeviceItem
)

CONFIG_SCHEMA = cv.All(
    light.light_schema(QuineticLight, light.LightType.BINARY)
    .extend(cv.COMPONENT_SCHEMA)
    .extend(DeviceItemBaseSchema)
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_OUTPUT_ID],
        config[CONF_ADDRESS],
    )
    await cg.register_component(var, config)
    await light.register_light(var, config)

    paren = await cg.get_variable(config[CONF_QUINETIC_ID])
    cg.add(var.set_parent(paren))
    cg.add(paren.add_device_item(var))
