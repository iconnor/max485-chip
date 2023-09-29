# Test MAX485 Chip

This chip will allow custom Modbus responses based on a lookup table

## Pin names

| Name | Description              |
| ---- | ------------------------ |
| RO   | Receiver Output          |
| RE   | Receiver Output Enable   |
| DE   | Driver Output Enable     |
| DI   | Driver Input             |
| VCC  | Positive Supply Voltage  |
| A    | Data Input / Output A    |
| B    | Data Input / Output B    |
| GND  | Ground                   |

## Usage

To use this chip in your project, include it and send it 9600 8N1 serial data. The chip will respond with the data in the lookup table.

```json
  "dependencies": {
    "chip-max485": "github:iconnor/max485-chip@0.0.0"
  }
```

Then, add the chip to your circuit by adding a `chip-max485` item to the `parts` section of diagram.json:

```json
  "parts": {
    ...,
    { "type": "chip-max485", "id": "chip1" }
  },
```

Credit goes also to uart examples provided by @urish - thanks!
