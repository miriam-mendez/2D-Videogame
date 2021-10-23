#ifndef _ANIMATION_KEYFRAMES
#define _ANIMATION_KEYFRAMES


#include <vector>


using namespace std;


// AnimKeyframes contains all information related to a single animation.
// These are the animation speed measured by millisecsPerKeyframe
// and texture coordinates for all keyframes.


struct AnimKeyframes {
    float millisecsPerKeyframe = 1000.f / 4.f;
    vector<glm::vec2> keyframeDispl;
    bool loop = true;
    int next = -1; // played if !loop and next > 0
};


#endif // _ANIMATION_KEYFRAMES


