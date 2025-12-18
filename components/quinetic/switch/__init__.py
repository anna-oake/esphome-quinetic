import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ADDRESS, CONF_ID

from .. import CONF_QUINETIC_ID, DeviceItem, DeviceItemBaseSchema, quinetic_ns

DEPENDENCIES = ["quinetic"]
CODEOWNERS = ["@anna-oake"]

QuineticSwitch = quinetic_ns.class_(
    "QuineticSwitch", cg.Component, switch.Switch, DeviceItem
)

CONFIG_SCHEMA = cv.All(
    switch.switch_schema(QuineticSwitch)
    .extend(cv.COMPONENT_SCHEMA)
    .extend(DeviceItemBaseSchema)
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_ADDRESS],
    )
    await cg.register_component(var, config)
    await switch.register_switch(var, config)

    paren = await cg.get_variable(config[CONF_QUINETIC_ID])
    cg.add(var.set_parent(paren))
    cg.add(paren.add_device_item(var))
