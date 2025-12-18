import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cc1101
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
)

CODEOWNERS = ["@anna-oake"]

DEPENDENCIES = ["cc1101"]

CONF_CC1101_ID = "cc1101_id"
CONF_QUINETIC_ID = "quinetic_id"

quinetic_ns = cg.esphome_ns.namespace("quinetic")
Quinetic = quinetic_ns.class_("Quinetic", cg.PollingComponent)

DeviceItem = quinetic_ns.struct("DeviceItem")
DeviceItemBaseSchema = cv.Schema(
    {
        cv.GenerateID(CONF_QUINETIC_ID): cv.use_id(Quinetic),
        cv.Required(CONF_ADDRESS): cv.hex_int_range(max=0xFFFFFFFF),
    },
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Quinetic),
            cv.GenerateID(CONF_CC1101_ID): cv.use_id(cc1101.CC1101Component),
        }
    ).extend(cv.polling_component_schema("30s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    radio = await cg.get_variable(config[CONF_CC1101_ID])
    cg.add(var.set_radio(radio))

    # force Quinetic RF profile
    cg.add(radio.set_packet_mode(True))
    cg.add(radio.set_packet_length(12))

    cg.add(radio.set_frequency(433.3e6))
    cg.add(radio.set_modulation_type(cc1101.Modulation.MODULATION_2_FSK))
    cg.add(radio.set_symbol_rate(100000))
    cg.add(radio.set_filter_bandwidth(135e3))
    cg.add(radio.set_fsk_deviation(50e3))

    cg.add(radio.set_sync_mode(cc1101.SyncMode.SYNC_MODE_16_16))
    cg.add(radio.set_sync1(0x10))
    cg.add(radio.set_sync0(0xD2))

    cg.add(radio.set_if_frequency(153e3))
    cg.add(radio.set_crc_enable(False))
    cg.add(radio.set_whitening(False))
    cg.add(radio.set_manchester(False))
    cg.add(radio.set_carrier_sense_above_threshold(False))
