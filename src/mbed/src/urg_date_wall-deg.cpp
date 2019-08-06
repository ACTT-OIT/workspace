#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Pose2D.h>

#include <tf/transform_broadcaster.h>
#include <tf/transform_datatypes.h>

#include "math.h"

#define POINT 726
#define C_POINT 384  //center

float dis[POINT];
float old_dis[POINT];
float radian[2];
float c_radian;   //center
float deg[2];  //measure 3 times, 2 points
float x[2], y[2];
float wall_deg_ave=0;
int old_n = 0;

geometry_msgs::Pose2D date;

class urg_date_node
{
private:
    ros::Subscriber sub_odom;
    ros::Subscriber sub_scan;

    ros::Publisher pub_urg;

    void cb_scan(const sensor_msgs::LaserScan::ConstPtr &urg)
    {
		int i = urg->ranges.size() / 2 - 44;

			int step = 0;
			int old_step = 1;
			char wall = 0;
			float wall_deg;
//			ROS_INFO("front-range : %0.3f", urg->ranges[C_POINT]);
//			int count = 0;
//			ROS_INFO("center-distance : %0.3f, %0.3f", urg->ranges[284], urg->ranges[484]);
			for(int n=284;n<484;n++) {  //front-range
				if(urg->ranges[n] < 0.7) {  //1m
					dis[n] = urg->ranges[n];
					if((dis[n]-dis[n-1])<0.05 && n!=483) step++; //1cm
					else if(step>=60) {
//						if(old_step==step) {
							deg[0] = 0.35 * (n-10) - 45.35;    // 0.3515625  45.3515625
							deg[1] = 0.35 * (n+10-step) - 45.35;  //step==wall size, 129 points==localization of 0[deg]
							for(int i=0;i<2;i++) radian[i] = deg[i] * M_PI / 180;
							x[0] = dis[n-10] * cos(radian[0]);
							x[1] = dis[n+10-step] * cos(radian[1]);
							y[0] = dis[n-10] * sin(radian[0]);
							y[1] = dis[n+10-step] * sin(radian[1]);			
		
							date.theta = atan2f(y[1]-y[0], x[1]-x[0]) * 180 / M_PI;
//							ROS_INFO("wall-deg : %0.1f, old_n : %d, step : %d", wall_deg, old_n, step);
							step = 0;
							old_n++;
//							}
						old_step = step;
					} else step = 0;
				} 
//				old_n = n;	
			}
	}


public:
    urg_date_node()
    {
      ros::NodeHandle nh("~"), nh1;

			sub_scan = nh.subscribe("/scan", 5, &urg_date_node::cb_scan, this);

			pub_urg = nh1.advertise<geometry_msgs::Pose2D>("/URG_msg", 1000);
    }
    void mainloop()
    {
        ros::Rate rate(30.0);

        while(ros::ok())
        {
            ros::spinOnce();

            pub_urg.publish(date);

            rate.sleep();
        }
        // ここに終了処理のコード
    }
};

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "urg_date_node");

    urg_date_node laser;

    laser.mainloop();
}

