#!/usr/bin/env python
#
# test_midiin_poll.py
#
"""Shows how to receive MIDI input by polling an input port."""

import logging
import sys
import time

import rtmidi
from rtmidi.midiutil import open_midiport

import serial

SERIAL_UPDATE_EVERY = 1 / 25.0
KNOBS = [
    [176, 1],
    [176, 2],
    [176, 3],
    [176, 4],
    [177, 1],
    [177, 2],
    [177, 3],
    [177, 4],
]

log = logging.getLogger('test_midiin_poll')

logging.basicConfig(level=logging.DEBUG)

port = sys.argv[1] if len(sys.argv) > 1 else None
try:
    midiin, port_name = open_midiport(port)
except (EOFError, KeyboardInterrupt):
    sys.exit()

s = serial.Serial("/dev/ttyACM0", baudrate=9600)

print("Entering main loop. Press Control-C to exit.")
try:
    changed_knobs = {}
    last_serial_update = 0
    while True:
        now = time.time()
        msg = midiin.get_message()

        # process midi package
        if msg:
            message, deltatime = msg
            #print("[%s] @%0.6f %r" % (port_name, now, message))
            if message[:2] in KNOBS:
                knob_index = KNOBS.index(message[:2])
                value = int((message[2] / 127.0) * 255)
                changed_knobs[knob_index] = value
                #print("knob %d: value %f" % (knob_index, value))

        # inform arduino about changes
        if len(changed_knobs) != 0 and now - last_serial_update > SERIAL_UPDATE_EVERY:
            print(changed_knobs)

            for knob_index, value in changed_knobs.items():
                s.write(bytearray([knob_index, value]))
            s.flushOutput()
            changed_knobs = {}
            last_serial_update = now

        while s.inWaiting() > 0:
            print(s.read().decode("ascii"), end="")

        time.sleep(0.01)
except KeyboardInterrupt:
    print('')
finally:
    print("Exit.")
    midiin.close_port()
    s.close()
    del midiin
