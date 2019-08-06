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
float x[2], y[2], l_x[2], l_y[2];
float wall_deg_ave=0;
int old_n = 0;
char flag = 0;

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
		int step = 0, l_step = 0;
		int old_step = 1, l_old_step = 1;
		float wall_deg;
		float min_dis = 5.0;
//		ROS_INFO("front-range : %0.3f", urg->ranges[C_POINT]);
//			int count = 0;
//		ROS_INFO("center-distance : %0.3f, %0.3f", urg->ranges[C_POINT], urg->ranges[484]);

		if(flag==0) date.x = urg->ranges[C_POINT];
		if(flag==0 && date.x<=0.7) flag = 1;
		ROS_INFO("min_dis : %0.3f", date.x);

		if(flag==1){
		for(int n=70;n<384;n++) {  //front-range -> 284 to 484
			if(urg->ranges[n] < 1.0) {  //1m
				if(((-0.05 < urg->ranges[n] - urg->ranges[n-1]) ||
				     urg->ranges[n] - urg->ranges[n-1] < 0.05)/* && n!=483 */) step++; //5cm
				if(step>=200 && (-3<step-old_step && step-old_step<3)) {
/*					for(int i=n-101;i<=n;i++) {
	                                        min_dis = urg->ranges[i];
	                                        date.x = min_dis;
					}*/
					deg[0] = 0.35 * (n-15) - 45.35;    // 0.3515625  45.3515625
					deg[1] = 0.35 * (n+15-step) - 45.35;  //step==wall size, 129 points==localization of 0[deg]
					for(int i=0;i<2;i++) radian[i] = deg[i] * M_PI / 180;
					x[0] = urg->ranges[n-15] * cos(radian[0]);
					x[1] = urg->ranges[n+15-step] * cos(radian[1]);
					y[0] = urg->ranges[n-15] * sin(radian[0]);
					y[1] = urg->ranges[n+15-step] * sin(radian[1]);
					date.theta = atan2f(y[1]-y[0], x[1]-x[0]) * 180 / M_PI;
					ROS_INFO("wall-deg : %0.3f, step : %d", date.theta, step);
//					ROS_INFO("deg[0] : %0.3f, [1] : %0.3f, n : %d, step : %d", deg[0], deg[1], n, step);
					step = 0;
				} else {old_n++;}
				old_step = step;

//				ROS_INFO("dis[270] : %0.3f, %0.3f", urg->ranges[270], urg->ranges[271]);

			}
		}
		if(date.theta<=-85.0) flag = 2;
		}
		if(flag==2) {
	                for(int n=44;n<284;n++) {  //front-range -> 284 to 484
	                        if(urg->ranges[n] < 0.7) {  //1m
	                                if(((-0.05 < urg->ranges[n] - urg->ranges[n-1]) ||
        	                             urg->ranges[n] - urg->ranges[n-1] < 0.05)/* && n!=483 */) l_step++; //5cm
                	                if(step>=125 && (-3<l_step-l_old_step && l_step-l_old_step<3)) {
                        	                deg[0] = 0.35 * (n-15) - 45.35;    // 0.3515625  45.3515625
	                                        deg[1] = 0.35 * (n+15-l_step) - 45.35;  //step==wall size, 129 points==localization of 0[d$
        	                                for(int i=0;i<2;i++) radian[i] = deg[i] * M_PI / 180;
                	                        l_x[0] = urg->ranges[n-15] * cos(radian[0]);
                        	                l_x[1] = urg->ranges[n+15-l_step] * cos(radian[1]);
                                	        l_y[0] = urg->ranges[n-15] * sin(radian[0]);
                                       		l_y[1] = urg->ranges[n+15-l_step] * sin(radian[1]);
                                        	date.theta = atan2f(l_y[1]-l_y[0], l_x[1]-l_x[0]) * 180 / M_PI;
	                                        ROS_INFO("left_wall-deg : %0.3f", date.theta);
//      	                                ROS_INFO("deg[0] : %0.3f, [1] : %0.3f, n : %d, step : %d", deg[0], deg[1], n, step);
                	                        l_step = 0;
                        	        } else {old_n++;}
				}
			}
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
        ros::Rate rate(10.0);

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

