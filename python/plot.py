import serial
import datetime
import random
import csv
import numpy
import time
import matplotlib.pyplot as plt

MEDIAN_FILTER_SIZE = 10

ser = serial.Serial()
ser.port = "/dev/ttyACM0"
ser.baudrate = 115200
ser.dsrdtr = True

fig = plt.figure()
ax = fig.add_subplot(111)
im = ax.imshow(numpy.zeros((8,8)), cmap="hot", interpolation="nearest")
history = numpy.empty((8,8,1))
plt.show(block=False)

im.set_clim(0,10)

ser.open()

fname = str(int(round(time.time() * 1000))) + ".log.csv"

# current_time = datetime.datetime.now()
with open(fname, 'w') as csv_file:
    writer = csv.writer(csv_file, delimiter=',')
    while True:
      data = ser.readline()
      frame = numpy.fromstring(data, sep=",").reshape((8,8)) * 0.25
      
      # Write to csv
      writer.writerow(frame.flatten())

      history = numpy.dstack((frame, history))[:,:,0:MEDIAN_FILTER_SIZE]
      im.set_array(frame - numpy.median(history, axis=2))
      fig.canvas.draw()
      # Print FPS
      #current_time = datetime.datetime.now()
      #print str(1000000/(current_time - last_time).microseconds) + " FPS"
      #last_time = current_time
  
