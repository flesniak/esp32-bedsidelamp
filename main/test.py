#!/usr/bin/env python3

leds_cnt = 12
leds_fade_in = 5
leds_on = 3
leds_fade_out = 5
total_iterations = 28

for iteration in range(total_iterations):
    leds = [0] * leds_cnt
    for led in range(leds_cnt):
        value = 0
        # n rising leds
        if led <= iteration and iteration - led < leds_fade_in:
            value = iteration - led + 1
        # n full leds
        elif iteration - led >= leds_fade_in and iteration - led < leds_fade_in + leds_on:
            value = 6
        # n falling leds
        elif iteration - led >= leds_fade_in + leds_on and iteration - led < leds_fade_in + leds_on + leds_fade_out:
            value = leds_fade_in + leds_on + leds_fade_out - iteration + led

        leds[led] = value

    print(f'iteration {iteration}: {leds}')