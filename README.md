# Inverter Chip example

Example of a MAX485 chip that pretends to talk to a Modbus slave over RS485, but actually responds via UART based on a lookup table.

The actual source code for the chip lives in [src/main.c](src/main.c), and the pins are described in [chip.json](chip.json).

## Building

The easiest way to build the project is to open it inside a Visual Studio Code dev container, and then run the `make` command.

## Testing

Working on that one...

## License

This project is licensed under the MIT license. See the [LICENSE](LICENSE) file for more details.
