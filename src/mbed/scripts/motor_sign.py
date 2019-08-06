#!/usr/bin/env python

import rospy
from std_msgs.msg import String

rospy.init_node('motor_sign')
pub = rospy.Publisher('chatter', String, queue_size=10)
rate = rospy.Rate(1)
while not rospy.is_shutdown():
	hello_str = String()
	hello_str.data = "hello %s" % rospy.get_time()
	pub.publish(hello_str)
	rate.sleep()
