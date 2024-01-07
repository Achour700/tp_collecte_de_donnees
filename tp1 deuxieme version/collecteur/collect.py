import serial as s

ser =s.Serial()
ser.baudrate = 9600
ser.port = "COM9"
ser.timeout = 10
ser.open()

while 1 :
    if (ser.in_waiting > 0):
        #print(ser.in_waiting)
        x = ser.readline().decode()
        print(x)
ser.close()