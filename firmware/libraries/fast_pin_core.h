#ifndef FAST_PIN_CORE_H
#define	FAST_PIN_CORE_H

// The Particle Photon API introduced a generic set of functions for fast pin-setting.
// This header provides a subset of those functions for the Particle Core as well.
#if (PLATFORM_ID == 0)

typedef uint16_t pin_t;

inline void pinSetFast(pin_t _pin)
{
    PIN_MAP[_pin].gpio_peripheral->BSRR = PIN_MAP[_pin].gpio_pin;
}

inline void pinResetFast(pin_t _pin)
{
    PIN_MAP[_pin].gpio_peripheral->BRR = PIN_MAP[_pin].gpio_pin;
}

#endif

#endif

