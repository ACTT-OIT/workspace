#!/usr/bin/env python
# coding: UTF-8

import rospy
import smbus
import pigpio
import time
import numpy as np
import serial
from geometry_msgs.msg import Twist

f = 50
servo_pin = 12

flag = 0

servo = pigpio.pi()
servo.set_mode(servo_pin, pigpio.OUTPUT)
servo.hardware_PWM(servo_pin, f, 89000)

s = serial.Serial('/dev/mbed', 9600)

def ster(deg):

	deg *= 39.1

        if deg < -39.1:
                servo.hardware_PWM(servo_pin, f, 102000)
        elif deg > 39.1:
                servo.hardware_PWM(servo_pin, f, 71000)
        else:
             servo.hardware_PWM(servo_pin, f, 89000-(deg*333.33))

def callback(pos):
        rospy.loginfo("angle.theta = %0.3f", pos.angular.z)
	d = pos.angular.z
	ster(d)
	pow = int(pos.linear.x * 127) + 127
	s.write(chr(pow))
        print('pow = %s' % pow)

def listener():
        rospy.init_node('servo_mpu', anonymous=True)
        rospy.Subscriber('/cmd_vel', Twist, callback)

        rospy.spin()

while(True):

        listener()

s.close()
