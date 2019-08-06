#!/usr/bin/env python
# coding: UTF-8

import rospy
import pigpio
import time
from geometry_msgs.msg import Pose2D

f = 50
servo_pin = 12

flag = 0

servo = pigpio.pi()
servo.set_mode(servo_pin, pigpio.OUTPUT)
servo.hardware_PWM(servo_pin, f, 89000)

def ster(deg):
        if deg < -39.1:
                servo.hardware_PWM(servo_pin, f, 102000)
        elif deg > 39.1:
                servo.hardware_PWM(servo_pin, f, 71000)
        elif deg < 0:
                servo.hardware_PWM(servo_pin, f, 89000-(deg*1.2*333.33))
        else:
                servo.hardware_PWM(servo_pin, f, 89000-(deg*333.33))

	'''
        elif deg >= 0:
                servo.hardware_PWM(servo_pin, f, 89000-(deg*460.358))
	'''

def callback(urg):

	'''
	ster(urg.theta)

	'''
	rospy.loginfo("urg.theta = %0.3f", urg.theta)

	global flag

	if flag==0 and urg.x < 0.7:
		flag = 1

	if flag==0:
		ster(0.0)
	elif flag==1:
		if urg.theta > -85:
			ster(39.0)
		else:
			flag = 2
	elif flag==2:
		rospy.loginfo("flag2 OOOOOOOH!")
		d = -90.0 + urg.theta
		ster(d)


def listener():
	rospy.init_node('servo', anonymous=True)
	rospy.Subscriber("/URG_msg", Pose2D, callback)

	rospy.spin()

while(True):

	listener()

