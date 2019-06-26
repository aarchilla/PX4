#include <ros/ros.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/WaypointPull.h>
#include <mavros_msgs/WaypointPush.h>
#include <mavros_msgs/WaypointClear.h>
#include <mavros_msgs/WaypointReached.h>
#include <mavros_msgs/WaypointList.h>
#include <mavros_msgs/CommandHome.h>
#include <iostream>
 
using namespace std;


mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg) {
    current_state = *msg;
}

mavros_msgs::WaypointReached reached_;
void ReachedCallback(const mavros_msgs::WaypointReached::ConstPtr& msg){
	ROS_INFO("\nReached\
                \nx: [%hu]", msg->wp_seq);
    reached_ = *msg;
	};

void printwaypoint(const mavros_msgs::WaypointList points)
{
    cout<<"count:"<<points.waypoints.size()<<endl;
    for (size_t i = 0; i < points.waypoints.size(); i++)
    {
        cout<<points.waypoints[i].x_lat<<" "<<points.waypoints[i].y_long<<" "<<points.waypoints[i].z_alt<<endl;
    }
    
}

mavros_msgs::WaypointList current_waypoints;
void get_waypoints(const mavros_msgs::WaypointList::ConstPtr& msg)
{
    current_waypoints = *msg;
    
    printwaypoint(current_waypoints);
}


int main (int argc, char **argv) {
	
	ros::init(argc,argv,"mission_planner");
	ros::NodeHandle nh;
	ros::NodeHandle p;
	ros::NodeHandle n;
											
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
										("mavros/state", 100, state_cb);
										
	ros::Subscriber reached_sub = nh.subscribe<mavros_msgs::WaypointReached>
          ("/mavros/mission/reached", 2, ReachedCallback);
                            									
	ros::Subscriber waypoints_sub = nh.subscribe<mavros_msgs::WaypointList>
									("mavros/mission/waypoints", 100, get_waypoints); 									    										                        
									    
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
                                        ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
                                         ("mavros/set_mode");
                                         
    ros::ServiceClient wp_clear_client = p.serviceClient<mavros_msgs::WaypointClear>
										("mavros/mission/clear");
	ros::ServiceClient wp_srv_client = n.serviceClient<mavros_msgs::WaypointPush>
										("mavros/mission/push");
                                         
    ros::Rate rate(20.0);     

    // Wait for FCU connection.
    while (ros::ok() && current_state.connected) {
        ros::spinOnce();
        rate.sleep();
    }
    
	  mavros_msgs::WaypointClear wp_clear_srv;
	  mavros_msgs::Waypoint wp_msg;
	  mavros_msgs::WaypointPush wp_push_srv;

      mavros_msgs::CommandBool arm_cmd;
	  arm_cmd.request.value = true;
	  
	  cout << "Desea añadir puntos de forma manual? Pulse 1 si es asi. Pulse otro numero para ejecutar la mision existente" << endl;
	  
	  int number;
	  
	  cin >> number; 
	  
	  if(number == 1) {
		  
			if (wp_clear_client.call(wp_clear_srv))
				{
				ROS_INFO("Lista de waypoints limpiada");
				}
			else
				{
				ROS_ERROR("Error en la limpieza de waypoints");
				}
			
			cout << "Numero de waypoints que desea poner" << endl;
			
			int num_waypoints;
			
			cin >> num_waypoints;
				
			vector<mavros_msgs::Waypoint> missionvector(num_waypoints);
			
			for(int is = 0;is < num_waypoints;++is) {
				
				 missionvector[is].frame = 3; // mavros_msgs::Waypoint::FRAME_GLOBAL;
				 missionvector[is].command = 16;
				 if(is == 0)
				 missionvector[is].is_current = true;
				 else
				 missionvector[is].is_current = false;
				 missionvector[is].autocontinue = true;
				 missionvector[is].param1 = 0;
				 missionvector[is].param2 = 0;
				 missionvector[is].param3 = 0;
				 missionvector[is].param4 = 0;
				 float x;
				 cout << "Añada X Coordenada" << endl;
				 cin >> x;
				 missionvector[is].x_lat =  x;
				 float y;
				 cout << "Añada la  Coordenada Y" << endl;
				 cin >> y;
				 missionvector[is].y_long = y;
				 float z;
				 cout << "Añada la Altura" << endl;
				 cin >> z;
				 missionvector[is].z_alt = z;
				 
			     wp_push_srv.request.waypoints.push_back(missionvector[is]);
					
				  if (wp_srv_client.call(wp_push_srv))
				  {
						ROS_INFO("Exito:%d", (bool)wp_push_srv.response.success);
				  }
				  else
				  {	
						ROS_ERROR("Waypoint no ha podido ser enviado");
						ROS_INFO("Exito:%d", (bool)wp_push_srv.response.success);
				  }
			}		 
		} 
		
		
				if(current_state.mode != "AUTO.MISSION") {
				ROS_INFO("AUTO SET");
				system("rosrun mavros mavsys mode -c AUTO.MISSION");
				}
				
							if (!current_state.armed) {
				if( arming_client.call(arm_cmd) &&
					arm_cmd.response.success) {
						ROS_INFO("Vehicle armado");
						}
				}	 	 
				
	  while(ros::ok())  {
			if((current_waypoints.waypoints.size()-1) == reached_.wp_seq && current_state.mode == "AUTO.MISSION") { 
				if(current_state.mode != "AUTO.RTL") {
				ROS_INFO("RETURN SET");
				system("rosrun mavros mavsys mode -c AUTO.RTL");
								} 
					}
					ros::spinOnce();
					rate.sleep();			
					}
	   return 0;	
}

