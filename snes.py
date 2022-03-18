from enum import Enum
import RPIO


class Pin(Enum):
    GPIO_17 = 17
    GPIO_27 = 27
    GPIO_22 = 22
    GPIO_05 = 5
    GPIO_06 = 6
    GPIO_19 = 19
    GPIO_26 = 26
    GPIO_21 = 21
    GPIO_20 = 20
    GPIO_16 = 16
    GPIO_25 = 25
    GPIO_24 = 24


class Button(Enum):
    A = "A"
    B = "B"
    X = "X"
    Y = "Y"
    RIGHT = "RIGHT"
    DOWN = "DOWN"
    LEFT = "LEFT"
    UP = "UP"
    SELECT = "SELECT"
    START = "START"
    L = "L"
    R = "R"


MAPPING = dict([
    (Pin.GPIO_17, Button.A),
    (Pin.GPIO_27, Button.B),
    (Pin.GPIO_22, Button.X),
    (Pin.GPIO_05, Button.Y),
    (Pin.GPIO_06, Button.RIGHT),
    (Pin.GPIO_19, Button.DOWN),
    (Pin.GPIO_26, Button.LEFT),
    (Pin.GPIO_21, Button.UP),
    (Pin.GPIO_20, Button.SELECT),
    (Pin.GPIO_16, Button.START),
    (Pin.GPIO_25, Button.L),
    (Pin.GPIO_24, Button.R)
])

# Handles initial state and button presses


class SNES():
    def __init__(self, radius):
        self.started = False

    def start(self):
        self.__clear_buttons()
        self.started = True

    def stop(self):
        self.__clean()
        self.started = False

    def press(self):
        if self.started == False:
            raise Exception("Please call start() first")

    def __clean():
        RPIO.cleanup()

    def __clear_buttons():
        for pin in MAPPING.keys():
            RPIO.setup(pin.value, RPIO.OUT, initial=RPIO.HIGH)
