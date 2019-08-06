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
        else:
                servo.hardware_PWM(servo_pin, f, 89000-(deg*333.33))
        '''
        elif deg >= 0:
                servo.hardware_PWM(servo_pin, f, 89000-(deg*460.358))
        '''

def callback(angle):
        rospy.loginfo("angle.theta = %0.3f", angle.theta)
	d = angle.theta
	ster(d)

def listener():
        rospy.init_node('servo_mpu', anonymous=True)
        rospy.Subscriber("/mpu6050_msg", Pose2D, callback)

        rospy.spin()

while(True):

        listener()

