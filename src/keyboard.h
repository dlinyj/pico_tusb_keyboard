/**
 * @file keyboard.h
 * @author @rktrlng
 * @brief create USB HID keycodes from pin inputs
 * @see https://github.com/rktrlng/pico_tusb_keyboard
 */

#ifndef KEYS_H
#define KEYS_H

#include "hardware/gpio.h" // gpio_*
#include "class/hid/hid.h" // HID_KEY_*


struct PinKey {
	const int pin;     // pico pin number
	const uint8_t key; // HID_KEY_*
};


class KeyBoard
{
private:
	// ===========================================================================
	// set these values to your situation
	const static int num_pins = 2; // state keeps track of max 32 pins (uint32_t)
	const PinKey pin_keys[num_pins] = { // connect pin to keycode
		{ 16, HID_KEY_A },
		{ 17, HID_KEY_ENTER }
	};
	// ===========================================================================

	uint32_t state = 0; // register for 32 pins

public:
	uint8_t key_codes[6] = { 0 }; // we can send max 6 keycodes per hid-report

	KeyBoard()
	{
		// set all pins to pulled down inputs
		for (size_t i = 0; i < num_pins; i++) {
			gpio_init(pin_keys[i].pin);
			gpio_pull_down(pin_keys[i].pin);
			gpio_set_dir(pin_keys[i].pin, GPIO_IN);
		}
	}

	virtual ~KeyBoard()
	{

	}

	bool update()
	{
		// clear keycodes
		for (size_t i = 0; i < 6; i++) {
			key_codes[i] = 0;
		}

		// read pins and set list of max 6 keycodes
		uint8_t index = 0;
		uint32_t new_state = 0;
		for (int i = 0; i < num_pins; i++) {
			int p = gpio_get(pin_keys[i].pin); // read pin for this key
			if (p == 1)	{ // pressed
				key_codes[index] = pin_keys[i].key; // set keycode
				new_state |= (1 << i); // set bit for the pin

				index++; // max is 6 keys at the time
				if (index >= 6) {
					break;
				}
			}
		}

		// something changed
		if (state != new_state) {
			return true;
		}

		// nothing changed
		return false;
	}
};


#endif /* KEYS_H */