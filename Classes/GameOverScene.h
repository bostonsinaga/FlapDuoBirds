#ifndef __GAME_OVER_SCENE_H__
#define __GAME_OVER_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"

class GameOverScene : public cocos2d::Layer {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene(cocos2d::RenderTexture *renTex, int currentScore_in);

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(GameOverScene);

    private:

        // METHODS //

        void onKeyReleasing();
        void touching();
        void goToMainMenuScene(float dt);

        // OBJECTS //

        Tool tool;
};

#endif // __GAME_OVER_SCENE_H__