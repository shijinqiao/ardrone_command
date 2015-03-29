#ifndef ARDRONEENUMSHPP
#define ARDRONEENUMSHPP

enum droneCurrentState
{
UNKNOWN_STATE = 0,
INITIALIZED = 1,
LANDED = 2,
FLYING = 3,
HOVERING = 4,
TEST = 5,
TAKING_OFF = 6,
LANDING = 7,
LOOPING = 8
};



enum LEDAnimationType
{
BLINK_GREEN_RED = 0, 
BLINK_GREEN = 1,
BLINK_RED = 2,
BLINK_ORANGE = 3,
SNAKE_GREEN_RED = 4,
FIRE = 5,
STANDARD = 6,
RED = 7,
GREEN = 8,
RED_SNAKE = 9,
BLANK = 10,
LEFT_GREEN_RIGHT_RED = 11,
LEFT_RED_RIGHT_GREEN = 12,
BLINK_STANDARD = 13
};


enum flightAnimationType
{
ARDRONE_ANIM_PHI_M30_DEG = 0, 
ARDRONE_ANIM_PHI_30_DEG = 1, 
ARDRONE_ANIM_THETA_M30_DEG = 2, 
ARDRONE_ANIM_THETA_30_DEG = 3,
ARDRONE_ANIM_THETA_20DEG_YAW_200DEG = 4, 
ARDRONE_ANIM_THETA_20DEG_YAW_M200DEG = 5, 
ARDRONE_ANIM_TURNAROUND = 6,
ARDRONE_ANIM_TURNAROUND_GODOWN = 7, 
ARDRONE_ANIM_YAW_SHAKE = 8, 
ARDRONE_ANIM_YAW_DANCE = 9, 
ARDRONE_ANIM_PHI_DANCE = 10,
ARDRONE_ANIM_THETA_DANCE = 11, 
ARDRONE_ANIM_VZ_DANCE = 12, 
ARDRONE_ANIM_WAVE = 13, 
ARDRONE_ANIM_PHI_THETA_MIXED = 14,
ARDRONE_ANIM_DOUBLE_PHI_THETA_MIXED = 15, 
ARDRONE_ANIM_FLIP_AHEAD = 16, 
ARDRONE_ANIM_FLIP_BEHIND = 17, 
ARDRONE_ANIM_FLIP_LEFT = 18,
ARDRONE_ANIM_FLIP_RIGHT = 19
};







#endif