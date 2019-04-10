# baremetal-sensorlogger

Authors: Paolo Rollo, Matricola 1713885 Ludovico Lowe, Matricola 1707235 Students @Sapienza, University of Rome.

Using bare metal programming in AVR C (so without intermediating with the Arduino IDE/Libraries), we are going to implement a system to read one or more sensors (like temperature..), and to deliver, when requested, the registrated data to the PC using a binary packet protocol. On the PC, we're going to implement a program to interact with the Arduino to: 
-set the parameters of the sensor(s). 
-send requests of getting the data registrated till the moment. 
-display the incoming messages.
