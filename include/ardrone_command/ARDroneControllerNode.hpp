#ifndef ARDRONECONTROLLERNODEHPP
#define ARDRONECONTROLLERNODEHPP

#include<mutex>
#include<thread>
#include<chrono>

#include "ros/ros.h"
#include "std_msgs/Empty.h"

#include "tagTrackingInfo.hpp"
#include "ARDroneEnums.hpp"
#include "command.hpp"

#include "SOMException.hpp"
#include "SOMScopeGuard.hpp"
#include "QRCodeStateEstimator.hpp"
#include "QRCodeBasedPoseInformation.hpp"

#include <std_msgs/UInt16.h>
#include <std_msgs/UInt32.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int16.h>
#include <std_msgs/UInt8.h>
#include <std_srvs/Empty.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Vector3.h>
#include "ardrone_command/serialized_ardrone_command.h"
#include "ardrone_command/altitude_control_state.h"
#include "ardrone_command/qr_code_state_info.h"
#include "ardrone_command/qr_go_to_point_control_info.h"
#include "ardrone_command/qr_orientation_control_info.h"
#include "ardrone_command/command_status_info.h"

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <ardrone_autonomy/vector31.h>
#include <ardrone_autonomy/vector21.h>
#include <ardrone_autonomy/matrix33.h>

#include <ardrone_autonomy/CamSelect.h>
#include <ardrone_autonomy/LedAnim.h>
#include <ardrone_autonomy/FlightAnim.h>
#include <ardrone_autonomy/RecordEnable.h>
#include <ardrone_autonomy/Navdata.h>

#include <queue>
#include <condition_variable>
#include <chrono>


//This defines how long to wait for a QR code sighting when in a mode reliant on QR code state estimation before automatically landing
#define SECONDS_TO_WAIT_FOR_QR_CODE_BEFORE_LANDING 1

//This defines how long to wait (in microseconds) for a QR code before considering the drone to be experiencing "high latency" and having it simply hover for a little while till things mellow out or it considers itself to have lost tracking
#define HIGH_LATENCY_WATER_MARK .3

//Topics that the drone publishes for public consumption
#define QR_CODE_STATE_PUBLISHER_STRING "/ardrone_command/qr_code_state_estimates"
#define ALTITUDE_CONTROL_PUBLISHER_STRING "/ardrone_command/altitude_control"
#define QR_CODE_GO_TO_POINT_CONTROL_PUBLISHER_STRING "/ardrone_command/go_to_point_control"
#define QR_CODE_ORIENTATION_CONTROL_PUBLISHER_STRING "/ardrone_command/orientation_control"
#define COMMAND_PROCESSING_INFO_PUBLISHER_STRING "/ardrone_command/command_processing"




/*
This object subscribes to a set of ROS information streams that are available regarding a AR drone (some of which may be aggregators of other streams) and publishes a command stream to control the drone using information gleamed from those streams.  This object also has commands available to launch the other nodes so that it may subscribe to them.
*/
class ARDroneControllerNode
{
public:
/*
This function takes the AR drone address and starts up the nessisary ROS nodes to be able to communicate with it.  If the string is empty, it uses the default AR drone address (currently only default functionality is implemented).  It also initializes the QR code based state estimation engine used for QR code based commands.
@param inputCameraImageWidth: The width of camera images used in the camera calibration
@param inputCameraImageHeight: The height of the camera images used in the camera calibration
@param inputCameraCalibrationMatrix: This is a 3x3 matrix that describes the camera transform (taking the distortion into account) in opencv format
@param inputDistortionParameters: a 1x5 matrix which has the distortion parameters k1, k2, p1, p2, k3
@param inputARDroneAddress: The address of the AR drone to connect to 

@exceptions: This function can throw exceptions
*/
ARDroneControllerNode(int inputCameraImageWidth, int inputCameraImageHeight, const cv::Mat_<double> &inputCameraCalibrationMatrix, const cv::Mat_<double> &inputCameraDistortionParameters, std::string inputARDroneAddress = "");

/*
This function allows a command to be added to the queue of commands for the drone to execute.  This function is threadsafe, so it can be used from multiple threads but may sometimes block.
@param inputCommand: The command to add

@exceptions: This function can throw exceptions
*/
void addCommand(const command &inputCommand);

/*
This function allows an outside thread to see how many commands there are in the current queue.  This does not count the one (if any) currently executing. This function is threadsafe (and so may sometimes block).
@return: The number of commands in the current queue

@exceptions: This function can throw exceptions
*/
int commandQueueSize();

/*
This function cleans up the object and waits for any threads the object spawned to return.
*/
~ARDroneControllerNode();


bool manualControlEnabled; //Not currently set up

friend void initializeAndRunSpinThread(ARDroneControllerNode *inputARDroneControllerNode);

//Only used as callback

/*
This function is used as a callback to handle navdata info.
@param inputMessage: The legacy nav-data message to handle 
*/
void handleNavData(const ardrone_autonomy::Navdata::ConstPtr &inputMessage);

/*
This function is used as a callback to handle images from the AR drone.
@param inputImageMessage: The image message to handle 
*/
void handleImageUpdate(const sensor_msgs::ImageConstPtr& inputImageMessage);

private:
/*
This threadsafe function copies the next available command in the buffer and returns false if there was no command to give.  Retrieved commands are remain in the buffer.
@param inputCommandBuffer: The next command in the queue or the same as before if no command was available
@return: True if there was a new command and false otherwise
*/
bool copyNextCommand(command &inputCommandBuffer);

/*
This threadsafe function deletes the next available command in the buffer.  If the buffer is empty, it returns without deleting.
*/
void popNextCommand();

/*
This threadsafe function removes all of the elements in the command queue
*/
void clearCommandQueue();

/*
This function is called to update the local cache of navdata information based on the given message.
@param inputMessage: The navdata message to update the cache with
*/
void updateNavdataCache(const ardrone_autonomy::Navdata::ConstPtr &inputMessage);

/*
This function sends a message to tell the drone to takeoff using its built in takeoff sequence
@exceptions: This function can throw exceptions
*/
void activateTakeoffSequence();

/*
This function sends a message to tell the drone to land using its built in landing sequence
@exceptions: This function can throw exceptions
*/
void activateLandingSequence();

/*
This function sends a message to tell the drone to active its emergency stop sequence (cutting power to the engines)
@exceptions: This function can throw exceptions
*/
void activateEmergencyStop();

/*
This function sends a message to set the linear and angular velocity of the drone
@param inputVelocityX: The X velocity of the drone
@param inputVelocityY: The Y velocity of the drone
@param inputVelocityZ: The Z velocity of the drone
@param inputRotationZ: The Z rotation rate of the drone
@exceptions: This function can throw exceptions
*/
void setVelocityAndRotation(double inputVelocityX, double inputVelocityY, double inputVelocityZ, double inputRotationZ);

/*
This function sets the linear and angular velocity of the drone to zero and enables the auto-hover mode to try to maintain its position
@exceptions: This function can throw exceptions
*/
void enableAutoHover();

/*
This function either sets the active camera to the front facing one or the bottom one
@param inputSetCameraFront: True if the front camera should be set active, false if the bottom camera should be

@exceptions: This function can throw exceptions
*/
void setCameraFront(bool inputSetCameraFront);

/*
This function will trigger an LED animation sequence on the quadcopter
@param inputAnimationType: The type of the animation
@param inputFrequency: The frequency of the animation (if blinking), in hertz
@param inputDuration: The duration of the animation in seconds

@exceptions: This function can throw exceptions
*/
void activateLEDAnimationSequence(LEDAnimationType inputAnimationType, double inputFrequency, int inputDuration);

/*
This function will trigger a flight animation
@param inputFlightAnimationType: What type of flight animation to perform

@exceptions: This function can throw exceptions
*/
void activateFlightAnimation(flightAnimationType inputFlightAnimationType);

/*
This function causes the drone to recalibrate its rotations using the assumption that it is on a flat service.  Don't use it when it is not on a flat surface.
@exceptions: This function can throw exceptions
*/
void calibrateFlatTrim();

/*
This function causes the drone to start recording to its USB stick (if it has one) or stop recording.
@param inputStartRecording: Set to true if the drone should start recording and false if it should stop
@exceptions: This function can throw exceptions
*/
void enableUSBRecording(bool inputStartRecording);

/*
This function returns true if the ARDrone has achieved the hovering state.
@return: True if the hovering state has been achieved
*/
bool checkIfHoveringStateAchieved();

/*
This function returns if the ARDrone has achieved the landed state.
@return: True if the landed state has been achieved
*/
bool checkIfLandedStateAchieved();

/*
This function checks if a tag has been spotted.
@return: True if there is one or more tags in the navdata cache
*/
bool checkIfTagSpotted();

/*
This function checks if the target altitude is reached.
@param inputNumberOfMillimetersToTarget: The wiggle room to match the target height (+- this amount from the target).  It is normally 10 mm.
*/
bool checkIfAltitudeReached(int inputNumberOfMillimetersToTarget = 10);

/*
This function checks to see if the last state estimate associated with a QR code identifier is either doesn't exist or is older than the given time.
@param inputQRCodeIdentifier: The identifier of the QR code that is defining the state estimate
@param inputSecondsBeforeStale: The number of seconds that can pass before an entry is considered stale
@return: true if the entry is stale and false otherwise
*/
bool checkIfQRCodeStateEstimateIsStale(const std::string &inputQRCodeIdentifier, double inputSecondsBeforeStale);

/*
This function checks if a command has been completed and should be removed from the queue.
@return: true if the command has been completed and false otherwise
*/
bool checkIfCommandCompleted(const command &inputCommand);

/*
This function removes the top command from the queue and does any state adjustment associated with the command being completed (such as marking the AR drone as having landed).
*/
void removeCompletedCommand();

/*
This function takes the appropriate actions for control given the current commands in the queue and data state.  It calls lower level functions to send out the appropriate commands.
*/
void processCurrentCommandsForUpdateCycle();

/*
This function adjusts and enables/disables low level behavior depending on the given command.
@param inputCommand: The command to execute.
@return: true if the command has been completed and false otherwise (allowing multiple commands to be executed in a single update cycle if they are instant).

@exceptions: This function can throw exceptions
*/
bool adjustBehavior(const command &inputCommand);

/*
This function takes care of low level behavior that depends on the specific state variables (such as reaching the desired altitude and orientation).

@exceptions: This function can throw exceptions
*/
void handleLowLevelBehavior();

QRCodeStateEstimator QRCodeEngine; //QR code state estimation engine used for QR code based commands

bool shutdownControlEngine;
bool controlEngineIsDisabled;
bool onTheGroundWithMotorsOff;
bool takingOff;
bool landing;
bool emergencyStopTriggered;
bool maintainAltitude;
bool homeInOnTag;
bool matchTagOrientation;
bool maintainQRCodeDefinedPosition;
bool maintainQRCodeDefinedOrientation;
int commandCounter; //How many commands have been completed
int lastPublishedCommandCount; //Number of last command that the object published about
double targetAltitude; //The altitude to maintain in mm
double targetAltitudeITerm;
double xHeading; //The current velocity settings of the drone
double yHeading; 
double xCoordinateITerm;
double yCoordinateITerm;
double currentAngularVelocitySetting; //The setting of the current velocity
std::vector<double> targetXYZCoordinate; //The target point to reach, if any
double QRTargetXITerm;
double QRTargetYITerm;
std::string targetXYZCoordinateQRCodeIdentifier; //The identifier of the QR code that defines the coordinate system for position holding
std::string targetOrientationQRCodeIdentifier; //The identifier of the QR code that we are trying to maintain orientation relative to (if any)
std::string QRCodeToSpotIdentifier; //Identifier of QR code to try to spot
bool currentlyWaiting;
std::chrono::time_point<std::chrono::high_resolution_clock> waitFinishTime; //When any current wait command is due to expire

//Mutex protected, only public access through threadsafe addCommand function and threadsafe commandQueueSize function
std::mutex commandMutex;
std::queue<command> commandQueue;

enum droneCurrentState state;  //The current state of the drone
double batteryPercent;  //Percentage of the drone's battery remaining
double rotationX; //Left/Right tilt in degrees
double rotationY; //Forward/backward tilt in degrees
double rotationZ; //Turn rotation estimate
double magneticX; //Magnetic reading in X
double magneticY; //Magnetic reading in Y
double magneticZ; //Magnetic reading in Z
double pressure;  //Pressure sensed by barometer (Pa)
double temperature; //Temperature reading
double windSpeed;  //Estimated wind speed
double windAngle; //Estimated wind angle
double windCompensationAngle;  //?
double altitude;   //Estimated altitude (mm)
double motorPWM[4];//Current PWM values for the motors
double velocityX;  //Current estimated X velocity (mm/s)
double velocityY;  //Current estimated Y velocity (mm/s)
double velocityZ;  //Current estimated Z velocity (mm/s)
double accelerationX;  //Current estimated X acceleration
double accelerationY;  //Current estimated Y acceleration
double accelerationZ;  //Current estimated Z acceleration
std::vector<tagTrackingInfo> trackedTags; //Information about any oriented roundel tags in the field of view
std::chrono::time_point<std::chrono::high_resolution_clock> navdataUpdateTime; //The timestamp of when the navdata update was received

std::vector<double> localTargetPointMovingAverage;

//Structure holding each of the most recently updated entries associated with a given QR code
std::map<std::string, std::unique_ptr<QRCodeBasedPoseInformation> > QRCodeIDToStateEstimate;


ros::NodeHandle nodeHandle;

bool spinThreadExitFlag; //This flag indicates if thread that is calling spinOnce should exit
std::unique_ptr<std::thread> spinThread;


ros::Subscriber navDataSubscriber; 
ros::Subscriber videoSubscriber; 

ros::Publisher takeOffPublisher;
ros::Publisher landingPublisher;
ros::Publisher emergencyStopPublisher;
ros::Publisher directionAndSpeedPublisher;

ros::ServiceClient cameraControlClient;
ros::ServiceClient setLEDAnimationClient;
ros::ServiceClient setFlightAnimationClient;
ros::ServiceClient calibrateFlatTrimClient;
ros::ServiceClient setUSBRecordingClient;

ros::Publisher QRCodeStateInfoPublisher;
ros::Publisher altitudeControlInfoPublisher;
ros::Publisher QRCodeGoToPointControlInfoPublisher;
ros::Publisher QRCodeOrientationControlInfoPublisher;
ros::Publisher commandProcessingInfoPublisher;
};


/*
This function repeatedly calls ros::spinOnce until the spinThreadExitFlag in the given object is set (usually by the object destructor.  It is usually run in a seperate thread.
@param inputARDroneControllerNode: The node this function call is associated
*/
void initializeAndRunSpinThread(ARDroneControllerNode *inputARDroneControllerNode);






#endif
