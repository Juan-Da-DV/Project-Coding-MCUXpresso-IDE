import bluepy.btle as btle  #Import BLE lybraries
import RPi.GPIO as GPIO     #GPIO library

#Pins assignment and variables declaration
buttonPin1 = 12
buttonPin2 = 13
Switch = 15
Serial_Out = 'Z'
Flag_A =' Set'

#GPIO setup
def setup():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(buttonPin1, GPIO.IN)
    GPIO.setup(buttonPin2, GPIO.IN)
    GPIO.setup(Switch, GPIO.IN)


def loop():            #Main program 
    Flag_A = 'set'     #Initial value for Flag_A
    while True:        #Program loop
        if GPIO.input(buttonPin1)==GPIO.LOW:
            Flag_A = 'Reset'
            p = btle.Peripheral("B0:B1:13:75:F3:AA")
            s = p.getServiceByUUID("0000ffe0-0000-1000-8000-00805f9b34fb")
            c = s.getCharacteristics()[0]
            c.write(bytes("A", "utf-8"))
            p.disconnect()
        if GPIO.input(buttonPin2)==GPIO.LOW:
            Flag_A = 'Reset'
            p = btle.Peripheral("B0:B1:13:75:F3:AA")
            s = p.getServiceByUUID("0000ffe0-0000-1000-8000-00805f9b34fb")
            c = s.getCharacteristics()[0]
            c.write(bytes("B", "utf-8"))
            p.disconnect()
        if GPIO.input(Switch)==GPIO.LOW:
            Flag_A = 'Reset'
            p = btle.Peripheral("B0:B1:13:75:F3:AA")
            s = p.getServiceByUUID("0000ffe0-0000-1000-8000-00805f9b34fb")
            c = s.getCharacteristics()[0]
            c.write(bytes("C", "utf-8"))
            p.disconnect()
        if GPIO.input(Switch)==GPIO.HIGH and GPIO.input(buttonPin1)==GPIO.HIGH and GPIO.input(buttonPin2)==GPIO.HIGH and Flag_A=='Reset':
            Flag_A = 'Set'
            p = btle.Peripheral("B0:B1:13:75:F3:AA")
            s = p.getServiceByUUID("0000ffe0-0000-1000-8000-00805f9b34fb")
            c = s.getCharacteristics()[0]
            c.write(bytes("D", "utf-8"))
            p.disconnect()
def destroy():
    GPIO.cleanup()
    
if __name__ == '__main__':
    print ('Program Start')
    setup()
    try:
        loop()
    except KeyboardInterrupt:
        destroy()
        
        
