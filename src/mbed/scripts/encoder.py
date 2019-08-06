#!/usr/bin/env python
# coding: UTF-8

import rospy
import pigpio
from geometry_msgs.msg import Pose2D

A = pigpio.pi()
B = pigpio.pi()

pinA = 4
pinB = 22

A.set_mode(pinA, pigpio.INPUT)
A.set_pull_up_down(pinA, pigpio.PUD_UP)

B.set_mode(pinB, pigpio.INPUT)
B.set_pull_up_down(pinB, pigpio.PUD_UP)

cnt = 0

class Publishers():

    def __init__(self):
        self.publisher = rospy.Publisher('/encod_msg', Pose2D, queue_size=500)

        self.message = Pose2D()
    '''
    def count(self, gpio, level, tick):
        Publishers().cnt = Publishers().cnt + 1
#        return self.cnt
        print(self.cnt)
    '''
    def make_msg(self): #, gpio, level, tick):
	global cnt
        cnt = cnt + 1
        self.message.x = cnt * 2.966 * 3.141592
        print(self.message)
	print(cnt)

    def send_msg(self):
        self.make_msg()
        self.publisher.publish(self.message)

def count(gpio, level, tick):
#    global cnt
#    cnt = cnt + 1
    Publishers().send_msg()
#    print(cnt)

cbA = A.callback(pinA, pigpio.FALLING_EDGE, count)
cbB = B.callback(pinB, pigpio.FALLING_EDGE, count)

def main():
    rospy.init_node('encod')

    pub = Publishers()

    rate = rospy.Rate(10)

    while not rospy.is_shutdown():
#        pub.send_msg()
        rate.sleep()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException: pass

