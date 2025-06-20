#include "../include.h"

#include "../datalayer/datalayer_extended.h"
#include "CanBattery.h"
#include "RS485Battery.h"

// These functions adapt the old C-style global functions battery-API to the
// object-oriented battery API.

// The instantiated class is defined by the pre-compiler define
// to support battery class selection at compile-time
#ifdef SELECTED_BATTERY_CLASS

Battery* battery = nullptr;
Battery* battery2 = nullptr;

void setup_battery() {
  // Instantiate the battery only once just in case this function gets called multiple times.
  if (battery == nullptr) {
    battery = new SELECTED_BATTERY_CLASS();
  }
  battery->setup();

#ifdef DOUBLE_BATTERY
  if (battery2 == nullptr) {
#if defined(BMW_I3_BATTERY)
    battery2 =
        new SELECTED_BATTERY_CLASS(&datalayer.battery2, &datalayer.system.status.battery2_allowed_contactor_closing,
                                   can_config.battery_double, WUP_PIN2);
#elif defined(KIA_HYUNDAI_64_BATTERY)
    battery2 = new SELECTED_BATTERY_CLASS(&datalayer.battery2, &datalayer_extended.KiaHyundai64_2,
                                          &datalayer.system.status.battery2_allowed_contactor_closing,
                                          can_config.battery_double);
#elif defined(SANTA_FE_PHEV_BATTERY) || defined(TEST_FAKE_BATTERY)
    battery2 = new SELECTED_BATTERY_CLASS(&datalayer.battery2, can_config.battery_double);
#else
    battery2 = new SELECTED_BATTERY_CLASS(&datalayer.battery2, nullptr, can_config.battery_double);
#endif
  }
  battery2->setup();
#endif
}

void update_values_battery() {
  battery->update_values();
}

// transmit_can_battery is called once and we need to
// call both batteries.
void transmit_can_battery(unsigned long currentMillis) {
  ((CanBattery*)battery)->transmit_can(currentMillis);

#ifdef DOUBLE_BATTERY
  ((CanBattery*)battery2)->transmit_can(currentMillis);
#endif
}

void handle_incoming_can_frame_battery(CAN_frame rx_frame) {
  ((CanBattery*)battery)->handle_incoming_can_frame(rx_frame);
}

#ifdef DOUBLE_BATTERY
void update_values_battery2() {
  battery2->update_values();
}

void handle_incoming_can_frame_battery2(CAN_frame rx_frame) {
  ((CanBattery*)battery2)->handle_incoming_can_frame(rx_frame);
}
#endif

#ifdef RS485_BATTERY_SELECTED
void transmit_rs485(unsigned long currentMillis) {
  ((RS485Battery*)battery)->transmit_rs485(currentMillis);
}

void receive_RS485() {
  ((RS485Battery*)battery)->receive_RS485();
}

#endif

#endif
