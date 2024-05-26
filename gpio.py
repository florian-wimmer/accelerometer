from gpiozero import LED
from time import sleep

pin = LED(17)

while True:
    pin.on()
    sleep(0.5)
    pin.off()
    sleep(0.5)