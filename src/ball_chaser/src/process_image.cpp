#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the drive_bot service to drive the robot in the specified directon
void drive_robot(float lin_x, float ang_z)
{
    // Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = (float)lin_x;
    srv.request.angular_z = (float)ang_z;

    // Call the drive_bot service to drive the robot in the specified directon
    if (!client.call(srv))
        ROS_ERROR("Failed to call service drive_bot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    
    // Splits img into 3 parts from left to right
    int third = img.step/3;
    
    // Checks whether white_pixel is found for the sake of stopping the robot if white pixels aren't on the screen
    bool white_pixel_found = false;

    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int i = 0; i < img.height * img.step; i+=3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
            white_pixel_found = true;
            if(i%img.step < third)
            	drive_robot(0.5, 1);
            else if (i%img.step < third*2)
            	drive_robot(0.5, 0);
            else
            	drive_robot(0.5, -1);
            break;
        }
    }
    if(!white_pixel_found)
        	drive_robot(0, 0);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from drive_bot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/drive_bot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
