#ifndef __PAUSE_SCENE_H__
#define __PAUSE_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"

class PauseScene : public cocos2d::Layer {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene(cocos2d::RenderTexture *renTex);

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(PauseScene);

    private:

        // METHODS //

        void touching();
        void onKeyReleasing();

        // pop game scene //

        void prepareToPopGameScene();
        void popGameScene(float dt);

        // OBJECTS //

        cocos2d::EventListenerTouchOneByOne *touchListener;
        cocos2d::EventListenerKeyboard *backButtonListener;

        cocos2d::DrawNode *darkScreen, *resumeButton, *homeButton;

        Tool tool;

        // VARIABLES //

        int pausedStartTime = std::time(0);
        bool isTouched = false, isTouchable = true;
        Vec2 touchBegan, touchEnded;
};

#endif // __PAUSE_SCENE_H__