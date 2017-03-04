import serial
import datetime
import random
import numpy
import matplotlib.pyplot as plt

ser = serial.Serial()
ser.port = "/dev/ttyACM0"
ser.baudrate = 115200
ser.dsrdtr = True

fig = plt.figure()
ax = fig.add_subplot(111)
im = ax.imshow(numpy.array([20.00 for x in range(64)]).reshape((8,8)))
plt.show(block=False)

im.set_clim(18, 37);

ser.open()

# last_time = datetime.datetime.now()
# current_time = datetime.datetime.now()
while True:
  data = ser.readline()
  im.set_array(numpy.fromstring(data, sep=",").reshape((8,8)) * 0.25)
  fig.canvas.draw()
  # Print FPS
  #current_time = datetime.datetime.now()
  #print str(1000000/(current_time - last_time).microseconds) + " FPS"
  #last_time = current_time
  
