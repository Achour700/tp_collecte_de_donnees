import serial as s


ser = s.Serial()
ser.baudrate = 9600
ser.port = "COM7"
ser.timeout = 10

ser.open()

while (1):
    
    data = ser.readline().decode()
    print(data)

ser.close()

