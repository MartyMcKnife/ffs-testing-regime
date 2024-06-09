# adapted from - https://forum.arduino.cc/t/pc-arduino-comms-using-python-updated/574496

import serial
import time

startMarker = '<'
endMarker = '>'
dataStarted = False
dataBuf = ""
messageComplete = False

# serial setup
def setupSerial(baudRate, serialPortName):
    
    global  serialPort
    
    serialPort = serial.Serial(port= serialPortName, baudrate = baudRate, timeout=0, rtscts=True)

    print("Serial port " + serialPortName + " opened  Baudrate " + str(baudRate))

    waitForArduino()

#arduino sender
def sendToArduino(stringToSend):
    
        # this adds the start- and end-markers before sending
    global startMarker, endMarker, serialPort
    
    stringWithMarkers = (startMarker)
    stringWithMarkers += stringToSend
    stringWithMarkers += (endMarker)

    serialPort.write(stringWithMarkers.encode('utf-8')) # encode needed for Python3


# largerly redundant, mainly to check if arduino is ready to receive signals in a stateful way
def recvLikeArduino():

    global startMarker, endMarker, serialPort, dataStarted, dataBuf, messageComplete

    if serialPort.inWaiting() > 0 and messageComplete == False:
        x = serialPort.read().decode("utf-8") # decode needed for Python3
        
        if dataStarted == True:
            if x != endMarker:
                dataBuf = dataBuf + x
            else:
                dataStarted = False
                messageComplete = True
        elif x == startMarker:
            dataBuf = ''
            dataStarted = True
    
    if (messageComplete == True):
        messageComplete = False
        return dataBuf
    else:
        return "XXX" 



# wait until the Arduino sends 'Arduino is ready' - allows time for Arduino reset
# it also ensures that any bytes left over from a previous message are discarded
def waitForArduino():
    print("Waiting for Arduino to reset")
     
    msg = ""
    while msg.find("Arduino is ready") == -1:
        msg = recvLikeArduino()
        if not (msg == 'XXX'): 
            print(msg)


if __name__ == "__main__":
    setupSerial(115200, "/dev/ttyACM0")
    with open("data.txt", 'r') as f:
        data = f.read().replace('\n', 'n')
        sendToArduino(data)
