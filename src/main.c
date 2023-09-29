// Wokwi Custom UART Chip Example
//
// This chip implements a simple ROT13 letter substitution cipher.
// It receives a string over UART, and returns the same string with
// each alphabetic character replaced with its ROT13 substitution.
//
// For information and examples see:
// https://link.wokwi.com/custom-chips-alpha
//
// SPDX-License-Identifier: MIT
// Copyright (C) 2022 Uri Shaked / wokwi.com

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
  uart_dev_t uart0;
  uint8_t modbusBuffer[256];
  uint16_t bufferIndex;
  uint64_t last_byte_time;
} chip_state_t;


static void on_uart_rx_data(void *user_data, uint8_t byte);
static void on_uart_write_done(void *user_data);
static void chip_timer_callback(void *user_data);

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  const uart_config_t uart_config = {
    .tx = pin_init("RO", INPUT_PULLUP),
    .rx = pin_init("DI", INPUT),
    .baud_rate = 9600,
    .rx_data = on_uart_rx_data,
    .write_done = on_uart_write_done,
    .user_data = chip,
  };
  chip->uart0 = uart_init(&uart_config);
  chip->bufferIndex = 0;
  chip->last_byte_time = 0;

  const timer_config_t t_config = {
    .callback = chip_timer_callback,
    .user_data = chip,
  };

  timer_t timer = timer_init(&t_config);
  uint32_t interval = 500;
  timer_start(timer, interval, true);

  printf("MAX485 Chip initialized!\n");
}

#define GET_KEY(fc, ah, al, qh, ql) ((uint64_t)(fc) << 40) | ((uint64_t)(ah) << 32) | ((uint64_t)(al) << 24) | ((uint64_t)(qh) << 16) | ((uint64_t)(ql) << 8)


static void process_modbus_frame(uart_dev_t uart0, uint8_t *buffer, uint16_t length) {
  // Create the key from buffer[1], buffer[2], and buffer[3]

  uint64_t key = GET_KEY(buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);

  printf("Working key 0x%016llX\n", key);

  // Use a switch-case to handle the different possible keys
  switch(key) {
    case GET_KEY(0x03, 0x00, 0x01, 0x00, 0x02): {
      uint8_t response[] = {0x03, 0x04, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
      uart_write(uart0, response, sizeof(response));
      break;
    }
    // Add more cases here
    default: {
      // Handle missing response, perhaps by sending a Modbus exception
    }
  }
}


static void chip_timer_callback(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  uint64_t current_time = get_sim_nanos() / 1000; // current time in nanoseconds

  // Check for 3.5 character time gap (approx. 3650 ns for 9600 baud)
  if (chip->last_byte_time && current_time - chip->last_byte_time >= 36500) {
    // Time gap detected, process the buffer
    if (chip->bufferIndex > 0) {
      printf("Time gap detected: %llu\n", current_time - chip->last_byte_time);
      printf("chip time %llu vs. current time %llu\n", chip->last_byte_time, current_time);
      printf("Buffer size: %hu b[0] = %hu\n", chip->bufferIndex, chip->modbusBuffer);
      process_modbus_frame(chip->uart0, chip->modbusBuffer, chip->bufferIndex);
    }
    chip->bufferIndex = 0;  // Reset buffer
  }
}

 static void on_uart_rx_data(void *user_data, uint8_t byte) {
  chip_state_t *chip = (chip_state_t*)user_data;

  // Save the byte to the buffer
  if (chip->bufferIndex < sizeof(chip->modbusBuffer)) {
    chip->modbusBuffer[chip->bufferIndex++] = byte;
  }

  // Update the last byte time
  uint64_t current_time = get_sim_nanos() / 1000; // current time in nanoseconds
  chip->last_byte_time = current_time;

}

static void on_uart_write_done(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  printf("MAX485 done\n");
}
