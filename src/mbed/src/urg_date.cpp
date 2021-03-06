#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>

#include <tf/transform_broadcaster.h>
#include <tf/transform_datatypes.h>

#include "math.h"

#define POINT 726
#define C_POINT 384  //center

float dis[POINT];
float old_dis[POINT];
float radian[3][2];
float c_radian;   //center
float deg[3][2];  //measure 3 times, 2 points
float x[3][2], y[3][2];
float wall_deg_ave=0;

class urg_date_node
{
private:
    ros::Subscriber sub_odom;
    ros::Subscriber sub_scan;

    ros::Publisher pub_twist;

    void cb_scan(const sensor_msgs::LaserScan::ConstPtr &urg)
    {
		int i = urg->ranges.size() / 2 - 44;
/*
		if(urg->ranges[i] < urg->range_min ||  //エラーの場合
		   urg->ranges[i] > urg->range_max ||  //範囲測定外の場合
           std::isnan(urg->ranges[i]))         //無限遠の場合
	    {
		    //ROS_INFO("front-range: measurement error");
			ROS_INFO("front-range : %0.3f", urg->ranges[C_POINT]);
	    } else {

*/
			int old_n = 0;
			int step = 0, old_step = 0;
			char wall = 0;
			float wall_deg[3];
//			ROS_INFO("front-range : %0.3f", urg->ranges[C_POINT]);
//			int count = 0;
			for(int n=44;n<POINT;n++) {
				if(urg->ranges[n] < 0.65) {  //1m
					dis[n] = urg->ranges[n];
					if((n-old_n)==1 && (dis[n]-dis[n-1])<0.0005) step++; //0.5cm
					else if((dis[n]-dis[n-1])>0.010) {
//						ROS_INFO("urg-point : %d , %d", old_step-step, step);
						if(step>=100) {
							wall++;
//							ROS_INFO("urg-point : %d", step);
							for(int m=0;m<3;m++) {
//								ROS_INFO("urg-point : %d", wall);
								for(int l=20;l>=0;l-=10) {
									deg[m][0] = 0.3515625 * (n-(step/2+l)-129);
									deg[m][1] = 0.3515625 * (n-(step/2+l-20)-129);  //step==wall size, 129 points==localization of 0[deg]
									for(int i=0;i<2;i++) radian[m][i] = deg[m][i] * M_PI / 180;
									
									x[m][0] = urg->ranges[n-(step/2+l)] * cos(radian[m][0]);
									x[m][1] = urg->ranges[n-(step/2+l-20)] * cos(radian[m][1]);
									y[m][0] = urg->ranges[n-(step/2+l)] * sin(radian[m][0]);
									y[m][1] = urg->ranges[n-(step/2+l-20)] * sin(radian[m][1]);			
	
									wall_deg[m] += atan2(y[m][1]-y[m][0], x[m][1]-x[m][0]) * 180 / M_PI;
										
//										deg[][] = 0.3515625 * (n-50-129);
//										radian[] = deg * 180 / M_PI;
//									ROS_INFO("urg-point : %f,  %f", deg[m][0], deg[m][1]);
								}
							wall_deg_ave = wall_deg[m] / 3.0;
							step = 0;
							}
						ROS_INFO("urg-point : %0.3f", wall_deg_ave);
						}
					} 
							
				}
				old_n = n;
			}
		}



// 		   ROS_INFO("urg-point   : %d", urg->ranges.size());
//		   ROS_INFO("front-range : %0.3f", urg->ranges[384]);
//		   ROS_INFO("front-radian: %0.3f", urg->angle_min);
//    }

public:
    urg_date_node()
    {
        ros::NodeHandle nh("~");
	
		sub_scan = nh.subscribe("/scan", 5, &urg_date_node::cb_scan, this);
    }
    void mainloop()
    {
        ros::Rate rate(10.0);
        while(ros::ok())
        {
            ros::spinOnce();

            //出力

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

