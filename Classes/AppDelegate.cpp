#include "AppDelegate.h"
#include "SplashScene.h"

// #define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
}

bool AppDelegate::applicationDidFinishLaunching() {

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("FlapDuoBirds", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("FlapDuoBirds");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    // director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    // director->setAnimationInterval(1.0f / 60);

    auto screenSize = glview->getFrameSize();
    auto uDef = UserDefault::getInstance();
    std::vector<std::string> searchPath;
    float scaleRate;

    searchPath.push_back("GRAPHICS");

    // match scale rate with suitable screen size //

    // QHD
    if (screenSize.width + screenSize.height > 3500) scaleRate = 1.0;
    // FHD
    else if (screenSize.width + screenSize.height > 2500) scaleRate = 0.75;
    // HD
    else if (screenSize.width + screenSize.height > 1500) scaleRate = 0.5;
    // SD
    else scaleRate = 0.375;

    uDef->setFloatForKey("SCALE RATE", scaleRate);
    uDef->flush();

    glview->setDesignResolutionSize(screenSize.width, screenSize.height, ResolutionPolicy::NO_BORDER);
    FileUtils::getInstance()->setSearchPaths(searchPath);

    // create a scene. it's an autorelease object
    auto scene = SplashScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}