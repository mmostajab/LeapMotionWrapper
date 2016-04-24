#include "LeapWrapper.h"
#include <windows.h>

#include "Log.h"

#define SHOW_LEAP_DEBUG_INFO
#define NEEDED

void SampleListener::onInit(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Leap Motion Initialized" << std::endl;
#endif
}

void SampleListener::onConnect(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Leap Motion Connected" << std::endl;
#endif
  
  //controller.enableGesture(Gesture::TYPE_CIRCLE);
  //controller.enableGesture(Gesture::TYPE_KEY_TAP);
  //controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  // Note: not dispatched when running in a debugger.
  LOG_INFO << "Disconnected" << std::endl;
#endif
}

void SampleListener::onExit(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Exited" << std::endl;
#endif
}

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  //LOG_INFO << "onFrame\n";
  const Frame frame = controller.frame();

#ifdef SHOW_LEAP_DEBUG_INFO
  if(frame.hands().count() > 0)
  LOG_INFO << "Frame id: " << frame.id()
            //<< ", timestamp: " << frame.timestamp()
            << ", hands: " << frame.hands().count()
            //<< ", extended fingers: " << frame.fingers().extended().count()
            //<< ", tools: " << frame.tools().count()
            << ", gestures: " << frame.gestures().count() << std::endl;
#endif

  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
	HandInfo handInfo;
	const Hand hand = *hl;
    
	handInfo.confidence = hand.confidence();
	handInfo.right = hand.isLeft() ? false : true;
#ifdef SHOW_LEAP_DEBUG_INFO
	std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
    LOG_INFO << std::string(2, ' ') << handType << ", id: " << hand.id()
              << ", palm position: " << hand.palmPosition() << std::endl;
#endif

#ifdef NEEDED
    // Get the hand's normal vector and direction
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();
#endif

#ifdef SHOW_LEAP_DEBUG_INFO
    // Calculate the hand's pitch, roll, and yaw angles
    LOG_INFO << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;
#endif

#ifdef NEEDED
    // Get the Arm bone
    Arm arm = hand.arm();
#endif

#ifdef SHOW_LEAP_DEBUG_INFO
    LOG_INFO << std::string(2, ' ') <<  "Arm direction: " << arm.direction()
              << " wrist position: " << arm.wristPosition()
              << " elbow position: " << arm.elbowPosition() << std::endl;
#endif

    // Get fingers
    const FingerList fingers = hand.fingers();
	for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
	  FingerInfo fingerInfo;
      const Finger finger = *fl;
	  fingerInfo.valid = true;
	  fingerInfo.type  = (FingerType)finger.type();


#ifdef SHOW_LEAP_DEBUG_INFO
    LOG_INFO << std::string(4, ' ') <<  fingerNames[finger.type()]
             << " finger, id: " << finger.id()
             << ", length: " << finger.length()
             << "mm, width: " << finger.width() << std::endl;
#endif

      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = finger.bone(boneType);
		BoneInfo boneInfo;
		boneInfo.type = (BoneType)boneType;

		for(int i  = 0; i < 3; i++) boneInfo.prevJoint[i] = bone.prevJoint()[i];
    for(int i  = 0; i < 3; i++) boneInfo.nextJoint[i] = bone.nextJoint()[i];
    for(int i  = 0; i < 3; i++) boneInfo.direction[i] = bone.direction()[i];
		fingerInfo.bones[b] = boneInfo;
#ifdef SHOW_LEAP_DEBUG_INFO
        LOG_INFO << std::string(6, ' ') <<  boneNames[boneType]
                  << " bone, start: " << bone.prevJoint()
                  << ", end: " << bone.nextJoint()
                  << ", direction: " << bone.direction() << std::endl;
#endif

      }

	  handInfo.fingers[fingerInfo.type] = fingerInfo;
    }
	while(!handsMutex.try_lock());
		handsInfo.push_back(handInfo);
	handsMutex.unlock();
  }

#ifdef ZERO
  // Get tools
  const ToolList tools = frame.tools();
  for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
    const Tool tool = *tl;
#ifdef SHOW_LEAP_DEBUG_INFO
    L_INFO << std::string(2, ' ') <<  "Tool, id: " << tool.id()
              << ", position: " << tool.tipPosition()
              << ", direction: " << tool.direction() << std::endl;
#endif
  }
#endif // ZERO

//  // Get gestures
//  const GestureList gestures = frame.gestures();
//  for (int g = 0; g < gestures.count(); ++g) {
//    Gesture gesture = gestures[g];
//
//    switch (gesture.type()) {
//      case Gesture::TYPE_CIRCLE:
//      {
//		  if(circleGestures.size() > MAX_GESTURE_BUFFER_SIZE) break;
//
//        CircleGesture circle = gesture;
//        std::string clockwiseness;
//
//        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
//          clockwiseness = "clockwise";
//        } else {
//          clockwiseness = "counterclockwise";
//        }
//
//        // Calculate angle swept since last frame
//        float sweptAngle = 0;
//        if (circle.state() != Gesture::STATE_START) {
//          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
//          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
//        }
//#ifdef SHOW_LEAP_DEBUG_INFO
//        LOG_INFO << std::string(2, ' ')
//                  << "Circle id: " << gesture.id()
//                  << ", state: " << stateNames[gesture.state()]
//                  << ", progress: " << circle.progress()
//                  << ", radius: " << circle.radius()
//                  << ", angle " << sweptAngle * RAD_TO_DEG
//                  <<  ", " << clockwiseness << std::endl;
//#endif
//
//		CircleGestureInfo lastCircleGesture;
//		lastCircleGesture.id    = gesture.id();
//		lastCircleGesture.state = (GestureState)gesture.state();
//		if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
//          lastCircleGesture.clockwise = true;
//        } else {
//          lastCircleGesture.clockwise = false;
//        }
//		lastCircleGesture.angle = sweptAngle * RAD_TO_DEG;
//		lastCircleGesture.progress = circle.progress();
//		lastCircleGesture.radius = circle.radius();
//
//		// wait until the mutex can be locked.
//		while(!circleMutex.try_lock());
//		    // push it to the queue
//			circleGestures.push_back( lastCircleGesture );
//		// unlock the mutex
//		circleMutex.unlock();
//
//        break;
//      }
//      case Gesture::TYPE_SWIPE:
//      {
//		  if(swipeGestures.size() > MAX_GESTURE_BUFFER_SIZE) break;
//
//        SwipeGesture swipe = gesture;
//#ifdef SHOW_LEAP_DEBUG_INFO
//        LOG_INFO << std::string(2, ' ')
//          << "Swipe id: " << gesture.id()
//          << ", state: " << stateNames[gesture.state()]
//          << ", direction: " << swipe.direction()
//          << ", speed: " << swipe.speed() << std::endl;
//#endif
//		  SwipeGestureInfo lastSwipeGesture;
//		  lastSwipeGesture.id		 = gesture.id();
//		  lastSwipeGesture.direction = leap2osg( swipe.direction() );
//		  lastSwipeGesture.state	 = (GestureState)gesture.state();
//		  lastSwipeGesture.speed	 = swipe.speed();
//
//		  // wait until the mutex can be locked.
//		  while(!swipeMutex.try_lock());
//		    // push it to the queue
//		  swipeGestures.push_back( lastSwipeGesture );
//		  // unlock the mutex
//		  swipeMutex.unlock();
//
//        break;
//      }
//      case Gesture::TYPE_KEY_TAP:
//      {
//		  if(keyTapGestures.size() > MAX_GESTURE_BUFFER_SIZE) break;
//
//        KeyTapGesture tap = gesture;
//#ifdef SHOW_LEAP_DEBUG_INFO
//        LOG_INFO << std::string(2, ' ')
//          << "Key Tap id: " << gesture.id()
//          << ", state: " << stateNames[gesture.state()]
//          << ", position: " << tap.position()
//          << ", direction: " << tap.direction()<< std::endl;
//#endif
//		  KeyTapGestureInfo lastKeyTapGesture;
//		  lastKeyTapGesture.id			= gesture.id();
//		  lastKeyTapGesture.state		= (GestureState)gesture.state();
//		  lastKeyTapGesture.position	= leap2osg( tap.position() );
//		  lastKeyTapGesture.direction	= leap2osg( tap.direction() );
//
//		  // wait until the mutex can be locked.
//		  while(!keyTapMutex.try_lock());
//		    // push it to the queue
//			keyTapGestures.push_back( lastKeyTapGesture );
//		  // unlock the mutex
//		  keyTapMutex.unlock();
//		  
//        break;
//      }
//      case Gesture::TYPE_SCREEN_TAP:
//      {
//		  if(screenTapGestures.size() > MAX_GESTURE_BUFFER_SIZE) break;
//
//		  ScreenTapGesture screentap = gesture;
//#ifdef SHOW_LEAP_DEBUG_INFO
//        LOG_INFO << std::string(2, ' ')
//          << "Screen Tap id: " << gesture.id()
//          << ", state: " << stateNames[gesture.state()]
//          << ", position: " << screentap.position()
//          << ", direction: " << screentap.direction()<< std::endl;
//#endif
//		  ScreenTapGestureInfo lastScreenTapGesture;
//		  lastScreenTapGesture.id			= gesture.id();
//		  lastScreenTapGesture.state		= (GestureState)gesture.state();
//		  lastScreenTapGesture.position		= leap2osg( screentap.position() );
//		  lastScreenTapGesture.direction	= leap2osg( screentap.direction() );
//
//		  // wait until the mutex can be locked.
//		  while(!screenTapMutex.try_lock());
//		    // push it to the queue
//			screenTapGestures.push_back( lastScreenTapGesture );
//		  // unlock the mutex
//		  screenTapMutex.unlock();
//
//        break;
//      }
//      default:
//#ifdef SHOW_LEAP_DEBUG_INFO
//        LOG_INFO << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
//#endif
//        break;
//
//    }
//  }

  if (!frame.hands().isEmpty() /*|| !gestures.isEmpty()*/) {
#ifdef SHOW_LEAP_DEBUG_INFO
    LOG_INFO << std::endl;
#endif

    if(handsInfo.size() == 1)
      if(handsInfo[0].right);
    else if(handsInfo.size() >= 2){
      HandInfo rightHand, leftHand;
      bool     foundRightHand = false, foundLeftHand = false;
      for(auto& handInfo : handsInfo){
        if(handInfo.right){
          rightHand = handInfo;
          foundRightHand = true;
        } else {
          leftHand = handInfo; 
          foundLeftHand = true;
        }
      }

      /*emit availableNewHandInfo(foundLeftHand, leftHand, foundRightHand, rightHand);*/
    } 
  }
}

 void SampleListener::onFocusGained(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Focus Gained" << std::endl;
#endif
}

void SampleListener::onFocusLost(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Focus Lost" << std::endl;
#endif
}

void SampleListener::onDeviceChange(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    LOG_INFO << "id: " << devices[i].toString() << std::endl;
    LOG_INFO << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
#endif
}

void SampleListener::onServiceConnect(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Service Connected" << std::endl;
#endif
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
#ifdef SHOW_LEAP_DEBUG_INFO
  LOG_INFO << "Service Disconnected" << std::endl;
#endif
}

std::vector< KeyTapGestureInfo > SampleListener::flushKeyTapGestureInfo() {
	std::vector< KeyTapGestureInfo > retGestures(0);

	while(!keyTapMutex.try_lock());
		retGestures = keyTapGestures;
		keyTapGestures.clear();
	keyTapMutex.unlock();

	return retGestures;
}

std::vector< ScreenTapGestureInfo > SampleListener::flushScreenTapGestureInfo() {
	std::vector< ScreenTapGestureInfo > retGestures(0);
	
	while(!screenTapMutex.try_lock());
		retGestures = screenTapGestures;
		screenTapGestures.clear();
	screenTapMutex.unlock();
	
	return retGestures;
}

std::vector< SwipeGestureInfo > SampleListener::flushSwipeGestureInfo() {
	std::vector< SwipeGestureInfo > retGestures(0);

	while(!swipeMutex.try_lock());
		retGestures = swipeGestures;
		swipeGestures.clear();
	swipeMutex.unlock();

	return retGestures;
}

std::vector< CircleGestureInfo > SampleListener::flushCircleGestureInfo() {
	std::vector< CircleGestureInfo > retGestures(0);

	while(!circleMutex.try_lock());
		retGestures = circleGestures;
		circleGestures.clear();
	circleMutex.unlock();

	return retGestures;
}

std::vector< HandInfo > SampleListener::flushHandsInfo() {
	std::vector< HandInfo > retGestures(0);

	while(!handsMutex.try_lock());
		retGestures = handsInfo;
		handsInfo.clear();
	handsMutex.unlock();

	return retGestures;
}

LeapWrapper::LeapWrapper(const LeapPolicyMode& policy) {
	controller.addListener(listener);
  
  switch(policy){
  case LEAP_POLICY_TABLE_MODE:
    controller.setPolicy(Leap::Controller::POLICY_DEFAULT);  
    break;

  case LEAP_POLICY_HMD_MODE:
    controller.setPolicy(Leap::Controller::POLICY_OPTIMIZE_HMD);  
    break;

  default:
    controller.setPolicy(Leap::Controller::POLICY_DEFAULT);  
  }
	
}

std::vector< KeyTapGestureInfo > LeapWrapper::flushKeyTapGestures() {
	return listener.flushKeyTapGestureInfo();
}

std::vector< ScreenTapGestureInfo > LeapWrapper::flushScreenTapGestures() {
	return listener.flushScreenTapGestureInfo();
}

std::vector< SwipeGestureInfo >	LeapWrapper::flushSwipeGestures() {
	return listener.flushSwipeGestureInfo();
}

std::vector< CircleGestureInfo > LeapWrapper::flushCircleGestures() {
	return listener.flushCircleGestureInfo();
}

std::vector< HandInfo > LeapWrapper::flushHands() {
	return listener.flushHandsInfo();
}

LeapWrapper::~LeapWrapper() {
	controller.removeListener(listener);
}