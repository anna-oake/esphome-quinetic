This is an ESPHome component that lets you control Quinetic relays and listen to Quinetic switches.

The component relies on the `cc1101` component and its packet mode.

The CC1101 is automatically configured with the required settings and cannot be shared with other components if used for Quinetic.

You only need to connect the SPI pins (CLK, MOSI, MISO, CS) and the GDO0 pin.

Packets from unconfigured devices are logged as warnings to help you discover their IDs.

Minimal usage example:

```yaml
spi:
  clk_pin: D5
  mosi_pin: D7
  miso_pin: D6

cc1101:
  cs_pin: 15
  gdo0_pin: 5

quinetic:

binary_sensor:
  - platform: quinetic
    address: 0x1337
    name: "That's a wall switch"

light:
  - platform: quinetic
    address: 0xDEADBEEF
    name: "A binary light"

switch:
  - platform: quinetic
    address: 0xB00B1337
    name: "Works as a switch too"
```

Wall switches can be added as binary sensors.

Relays and relay-switch combos can be added as lights or switches.
Their states will be automatically polled every 30 seconds.

The polling interval for relays and relay-switches can be configured like this:
```yaml
quinetic:
  update_interval: 45s
```

# Credits

**Huge thanks to @ReFil (and earlier upstream contributors) for the [Kinetic2MQTT](https://github.com/ReFil/Kinetic2MQTT2) bridge I heavily relied on developing this component.**
