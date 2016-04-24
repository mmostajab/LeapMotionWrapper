#ifndef __LEAP_WRAPPER_H__
#define __LEAP_WRAPPER_H__

// Leap 
#include <Leap.h>

// STD
#include <vector>
#include <mutex>


enum GestureState {
  GESTURE_STATE_INVALID = 0,
  GESTURE_STATE_START   = 1,
  GESTURE_STATE_UPDATE  = 2,
  GESTURE_STATE_END     = 3
};

enum FingerType {
  FingureType_Thumb 	= 0,
  FingureType_Index	  = 1,
  FingureType_Middle	= 2,
  FingureType_Ring	  = 3,
  FingureType_Pinky	  = 4
};

enum BoneType {
  BoneType_Metacarpal		= 0,
  BoneType_Proximal		  = 1,
  BoneType_Middle			  = 2,
  BoneType_Distal			  = 3
};

struct BoneInfo {
  BoneType type;
  float prevJoint[3];
  float nextJoint[3];
  float direction[3];
};

struct FingerInfo {
  bool        valid;
  FingerType  type;
  BoneInfo    bones[4];

  FingerInfo(): valid(false) {}
};

struct HandInfo {
  bool        right;
  float       confidence;
  FingerInfo  fingers[5];
};

struct SwipeGestureInfo {
  int				    id;
  GestureState	state;
  float		      direction[3];
  float			    speed;
};

struct KeyTapGestureInfo {
  int				    id;
  GestureState	state;
  float		      position [3];
  float     		direction[3];
};

struct ScreenTapGestureInfo {
  int				    id;
  GestureState	state;
  float		      position [3];
  float		      direction[3];
};

struct CircleGestureInfo {
  int				    id;
  GestureState	state;
  bool			    clockwise;
  float			    progress;
  float			    radius;
  float		  	  angle;
};

#define MAX_GESTURE_BUFFER_SIZE 64

using namespace Leap;

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

class SampleListener : public Listener {

  public:

    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

	  std::vector< KeyTapGestureInfo		>	flushKeyTapGestureInfo();
	  std::vector< ScreenTapGestureInfo	>	flushScreenTapGestureInfo();
	  std::vector< SwipeGestureInfo		  >	flushSwipeGestureInfo();
	  std::vector< CircleGestureInfo		>	flushCircleGestureInfo();
	  std::vector< HandInfo				      > flushHandsInfo();

  private:
	  
	  std::vector< KeyTapGestureInfo	> keyTapGestures;
	  std::vector< ScreenTapGestureInfo > screenTapGestures;
	  std::vector< SwipeGestureInfo		> swipeGestures;
	  std::vector< CircleGestureInfo	> circleGestures;
	  
	  std::vector< HandInfo	> handsInfo;

	  std::mutex keyTapMutex, screenTapMutex, swipeMutex, circleMutex;
	  std::mutex handsMutex;
};

enum LeapPolicyMode {
  LEAP_POLICY_TABLE_MODE = 0,
  LEAP_POLICY_HMD_MODE   = 1
};

class LeapWrapper {

public:
	LeapWrapper(const LeapPolicyMode& policy = LEAP_POLICY_TABLE_MODE);

	std::vector< KeyTapGestureInfo		>	flushKeyTapGestures();
	std::vector< ScreenTapGestureInfo	>	flushScreenTapGestures();
	std::vector< SwipeGestureInfo		  >	flushSwipeGestures();
	std::vector< CircleGestureInfo		>	flushCircleGestures();
	
	std::vector< HandInfo > flushHands();

	void toWorldCoords();

	~LeapWrapper();

private:
	SampleListener listener;
	Controller     controller;

};

#endif // __LEAP_WRAPPER_H__