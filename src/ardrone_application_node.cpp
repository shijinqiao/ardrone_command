#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include <unistd.h>
#include "ARDroneControllerNode.hpp"



int main(int argc, char** argv)
{

printf("Test 1\n");
printf("Initializing ROS\n");
ros::init(argc, argv, "droneTime");

//Define image size
//int imageWidth = 1280;
//int imageHeight = 720;
int imageWidth = 640;
int imageHeight = 360;

//Define camera matrix according to webcam calibration (from OpenCV camera calibration file)
//Scaled values due to the ARDrone's resolution change (from ardrone_autonomy having bad default without option to change)
cv::Mat_<double> cameraMatrix(3, 3);
cameraMatrix.at<double>(0,0) =  1.1485540667575478e+03*.5;
cameraMatrix.at<double>(0,1) = 0.;
cameraMatrix.at<double>(0,2) = 6.3950000000000000e+02*.5;
cameraMatrix.at<double>(1,0) =  0.;
cameraMatrix.at<double>(1,1) = 1.1485540667575478e+03*.5;
cameraMatrix.at<double>(1,2) = 3.5950000000000000e+02*.5;
cameraMatrix.at<double>(2,0) = 0.0;
cameraMatrix.at<double>(2,1) = 0.0; 
cameraMatrix.at<double>(2,2) = 1.0;
    

//Define distortion parameters according to webcam calibration (from OpenCV camera calibration file)
cv::Mat_<double> distortionParameters(1, 5); //k1, k2, p1, p2, k3
distortionParameters.at<double>(0, 0) = -5.2981696528785138e-01;
distortionParameters.at<double>(0, 1) = 3.3890562816386838e-01;
distortionParameters.at<double>(0, 2) =  0.0;
distortionParameters.at<double>(0, 3) =  0.0;
distortionParameters.at<double>(0, 4) = -1.1542908311868905e-01;


std::unique_ptr<ARDroneControllerNode> myARDroneControllerNode;
try
{
printf("Initializing controller node\n");
myARDroneControllerNode.reset(new ARDroneControllerNode(imageWidth, imageHeight, cameraMatrix, distortionParameters));
printf("Initialization completed\n");
}
catch(const std::exception &inputException)
{
fprintf(stderr, "%s\n", inputException.what());
}

command commandTakeoff;
commandTakeoff.setTakeoffCommand();

printf("Adding takeoff command\n");
myARDroneControllerNode->addCommand(commandTakeoff);
printf("Takeoff command added\n");


command commandSetTargetAltitude;
commandSetTargetAltitude.setTargetAltitudeCommand(500.0);
myARDroneControllerNode->addCommand(commandSetTargetAltitude);

command commandWaitUntilTargetAltitudeReached;
commandWaitUntilTargetAltitudeReached.setWaitUntilAltitudeReached(10.0);
myARDroneControllerNode->addCommand(commandWaitUntilTargetAltitudeReached);

command commandWaitForQRCode;
commandWaitForQRCode.setWaitUntilSpecificQRCodeIsSpottedCommand("BigQRCode", 3.0);
myARDroneControllerNode->addCommand(commandWaitForQRCode);

command commandLookAtQRCodePoint;
commandLookAtQRCodePoint.setMaintainOrientationTowardSpecificQRCode("BigQRCode");
myARDroneControllerNode->addCommand(commandLookAtQRCodePoint);

command commandGoToQRCodePoint;
commandGoToQRCodePoint.setMaintainPositionAtSpecificQRCodePoint("BigQRCode", 0.0, 0.0, 3.0);
myARDroneControllerNode->addCommand(commandGoToQRCodePoint);


command QRCodePointWait1; QRCodePointWait1.setWaitUntilPositionAtSpecificQRCodePointReachedCommand(10.0);
myARDroneControllerNode->addCommand(QRCodePointWait1);

command commandWait;
commandWait.setWaitCommand(500.0);
myARDroneControllerNode->addCommand(commandWait);


/*
command commandWait;
commandWait.setWaitCommand(500.0);
myARDroneControllerNode->addCommand(commandWait);



command commandWaitForQRCode;
commandWaitForQRCode.setWaitUntilSpecificQRCodeIsSpottedCommand("BigQRCode", 3.0);
myARDroneControllerNode->addCommand(commandWaitForQRCode);
*/


/*
command commandLookAtQRCodePoint;
commandLookAtQRCodePoint.setMaintainOrientationTowardSpecificQRCode("BigQRCode");
myARDroneControllerNode->addCommand(commandLookAtQRCodePoint);
*/


//Go to -1.0, 0 4.0
/*
command commandGoToQRCodePoint;
commandGoToQRCodePoint.setMaintainPositionAtSpecificQRCodePoint("BigQRCode", 0.0, -0.5, 4.0);
myARDroneControllerNode->addCommand(commandGoToQRCodePoint);
*/

command commandSetHorizontalHeading;
commandSetHorizontalHeading.setHorizontalHeadingCommand(0.05, 0.0);
myARDroneControllerNode->addCommand(commandSetHorizontalHeading);

/*
command commandWait;
commandWait.setWaitCommand(2.0);
myARDroneControllerNode->addCommand(commandWait);
*/

command commandSetAngularVelocity;
commandSetAngularVelocity.setAngularVelocityCommand(0.5);
myARDroneControllerNode->addCommand(commandSetAngularVelocity);


command commandWait0;
commandWait0.setWaitCommand(6.0);
myARDroneControllerNode->addCommand(commandWait0);



command commandLanding;
commandLanding.setLandingCommand();
printf("Adding landing command\n");
myARDroneControllerNode->addCommand(commandLanding);
printf("Landing command added\n");



/*
command commandSetHorizontalHeading;
commandSetHorizontalHeading.setHorizontalHeadingCommand(0.025, 0.0);
myARDroneControllerNode->addCommand(commandSetHorizontalHeading);

command commandSetAngularVelocity;
commandSetAngularVelocity.setAngularVelocityCommand(0.5);
myARDroneControllerNode->addCommand(commandSetAngularVelocity);

//Go to -1.0, 0 4.0
command commandGoToQRCodePoint;
commandGoToQRCodePoint.setMaintainPositionAtSpecificQRCodePoint("BigQRCode", -1.0, 0.0, 4.0);
myARDroneControllerNode->addCommand(commandGoToQRCodePoint);
*/


/*

//command commandWaitUntilTagSpotted;
//commandWaitUntilTagSpotted.setWaitUntilTagIsSpottedCommand(6.0);
//myARDroneControllerNode->addCommand(commandWaitUntilTagSpotted);

command commandSetHorizontalHeading2;
commandSetHorizontalHeading2.setHorizontalHeadingCommand(0.00, 0.0);
myARDroneControllerNode->addCommand(commandSetHorizontalHeading2);



//command commandSetHomeInOnTag;
//commandSetHomeInOnTag.setHomeInOnTagCommand();
//myARDroneControllerNode->addCommand(commandSetHomeInOnTag);


command commandWait;
commandWait.setWaitCommand(16.0);
myARDroneControllerNode->addCommand(commandWait);











*/

while(myARDroneControllerNode->commandQueueSize() > 0)
{
}

return 0;
}

