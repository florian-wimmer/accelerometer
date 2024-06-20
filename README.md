# Acceleration Sensor
The C++ Programm, connect the Raspberry PI to the LSM6DSO 3D accelerometer and 3D gyroscope.
The Programm also tracks the input of simple pressure switches.

The data analysation is done in Python.

## C++ Programm
To make the Programm, call the following command in the terminal in the same folder as the Makefile:
```
make
```
To run the Project, execute:
```
make run
```

## Measurement
The measured data is saved within `measurement/data`.

And can be drawn with the help of a python programm which can be found in `python` folder.

Execute the python programm within the python folder (otherwise the path will not match up), with the following command:
```
python drawMeasurement.py
```