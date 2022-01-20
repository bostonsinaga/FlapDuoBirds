#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"

class GameScene : public cocos2d::LayerGradient {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene();

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(GameScene);

    private:

        ////// METHODS //////

        //// main ////

        void setPhysicsWorld(cocos2d::PhysicsWorld *world) {sceneWorld = world;}
        void contacting();
        void touching();
        void onKeyReleasing();

        // bird //

        void birdFalls_1(float dt);
        void birdFalls_2(float dt);
        void birdFlaps(int tag);
        void birdFlaps_schedule(float dt);
        void atCollideBird(int tag);
        void redrawBird(int tag);
        void generateBirdBody(int tag);

        // obstacle //

        void obsSpawn(float dt);
        void buildingSpawn(float dt);

        // gui //

        void respawnCountdown_1(float dt);
        void respawnCountdown_2(float dt);
        void respawnCountdown(int tag);
        void scoreMainMenu();
        void scoreUp(int tag, bool isAction);
        void generateTitleScore(cocos2d::Size scoreBarSize, bool isMainMenu);
        void removeScoreTitle(float dt);
        int getScoreDigitCount(int score);

        // scenes //

        void startGame();
        void checkingTutorialNeeds();
        void tut_tut1(float dt);
        void goToGameScene(float dt);
        cocos2d::RenderTexture *getRenTex();

        ////// OBJECTS //////

        cocos2d::PhysicsWorld *sceneWorld;
        cocos2d::Vec2 touchBegan, touchEnded;

        cocos2d::DrawNode *bird[2], *birdWing[2][3], *playButton;
        cocos2d::Sprite *carnage[2][6];
        cocos2d::Label *countLabel;
        cocos2d::PhysicsBody *birdBody[2];

        cocos2d::Color4F birdColors[10] = {

            cocos2d::Color4F::GRAY,
            cocos2d::Color4F::RED,
            cocos2d::Color4F::GREEN,
            cocos2d::Color4F::BLUE,
            cocos2d::Color4F::ORANGE,
            cocos2d::Color4F::MAGENTA,
            cocos2d::Color4F(0.0, 1.0, 1.0, 1.0),
            cocos2d::Color4F(1.0, 0.0, 0.5, 1.0),
            cocos2d::Color4F(0.5, 0.0, 0.5, 1.0),
            cocos2d::Color4F(0.5, 0.0, 1.0, 1.0)
        };

        struct SCOREBAR {

            cocos2d::DrawNode *bg, *tlbg;
            cocos2d::Label *lb, *tl, *cllb;
        } scoreBar;

        Tool tool;

        ////// VARIABLES //////

        const int bird_colMask[2] = {1, 2}, obs_colMask = 3, gap_colMask = 4;
        const float flapTime = 0.25;

        int fallenBirdsCount = 0, countDown = 5, currentScore = -1;
        float obsSpawnTimeDistance;
        bool isShow[2] = {true, true}, isGameStarted = false, isUncelebrated = true, isCelebrating = false;
};

#endif // __GAME_SCENE_H__