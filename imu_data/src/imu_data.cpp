#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include <nav_msgs/Odometry.h>
#include <std_msgs/String.h> 
#include <stdio.h>
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/Vector3Stamped.h"
#include <mavros_msgs/CommandBool.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/State.h>
#include <nav_msgs/Odometry.h>
#include <visualization_msgs/Marker.h>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

geometry_msgs::PoseStamped pose;
double alt;
 
void chatterCallback(const sensor_msgs::Imu::ConstPtr& msg){
    ROS_INFO("\nlinear acceleration\
                \nx: [%f]\ny:[%f]\nz:[%f]", msg->linear_acceleration.x,
                msg->linear_acceleration.y, msg->linear_acceleration.z);
}

void local_pos(const geometry_msgs::PoseStamped &msg)
{
	pose.pose.position.x = msg.pose.position.x;
    pose.pose.position.y = msg.pose.position.y;
    alt = msg.pose.position.z;

}
using namespace std;

int main(int argc, char **argv){
    ros::init(argc, argv, "imu_data");
    ros::NodeHandle n;
    ros::Subscriber local;
    ros::Subscriber sub = n.subscribe("/mavros/imu/data", 1000, chatterCallback);
    ros::Publisher vis_pub = n.advertise<visualization_msgs::Marker>( "visualization_marker", 1 );
    local = n.subscribe("/mavros/local_position/pose", 1, local_pos);
    
    ros::Rate rate(20.0);
    
    
    while (ros::ok())
		{
		visualization_msgs::Marker marker;
		marker.header.frame_id = "base_link";
		marker.header.stamp = ros::Time();
		marker.ns = "my_namespace";
		marker.id = 0;
		marker.type = visualization_msgs::Marker::SPHERE;
		marker.action = visualization_msgs::Marker::ADD;
		marker.pose.position.x = pose.pose.position.x;
		marker.pose.position.y = pose.pose.position.y;
		marker.pose.position.z = alt;
		cout << pose.pose.position.x << endl;
		cout << alt << "asd" << endl;
		marker.pose.orientation.x = 0.0;
		marker.pose.orientation.y = 0.0;
		marker.pose.orientation.z = 0.0;
		marker.pose.orientation.w = 1.0;
		marker.scale.x = 1;
		marker.scale.y = 0.1;
		marker.scale.z = 0.1;
		marker.color.a = 1.0; // Don't forget to set the alpha!
		marker.color.r = 0.0;
		marker.color.g = 1.0;
		marker.color.b = 0.0;
		//only if using a MESH_RESOURCE marker type:
		marker.mesh_resource = "package://mavlink_sitl_gazebo/models/rotors_description/meshes/iris.stl";
		vis_pub.publish( marker );
		ros::spinOnce();
        rate.sleep();
		}
	return 0;
}
