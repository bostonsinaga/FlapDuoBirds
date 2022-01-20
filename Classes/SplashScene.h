#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"

class SplashScene : public cocos2d::LayerColor {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene();

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(SplashScene);

    private:

        // METHODS //

        void goToTitleScene(float dt);

        // OBJECTS //

        Tool tool;
};

#endif // __SPLASH_SCENE_H__