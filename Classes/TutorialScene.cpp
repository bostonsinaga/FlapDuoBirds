#include "TutorialScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_ts;

Scene* TutorialScene::createScene(cocos2d::RenderTexture *renTex) {

    screenShot_ts = renTex;

    auto scene = Scene::create();
    auto layer = TutorialScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool TutorialScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    screenShot_ts->setPosition(tool.ctrPos());
    this->addChild(screenShot_ts);

    ////////////// stuff /////////////////

    // dark screen //

    darkScreen = DrawNode::create();
    darkScreen->setPosition(tool.ctrPos());
    darkScreen->drawSolidRect(

        Vec2(-tool.getVSz().width / 2, -tool.getVSz().height / 2),
        Vec2(tool.getVSz().width / 2, tool.getVSz().height / 2),
        Color4F(0, 0, 0, 100)
    );
    this->addChild(darkScreen);

    // label
    label = tool.myLabel("TAP TO FLY");
    label->setPosition(tool.pos(0.5, 0.75));
    label->enableBold();
    label->setOpacity(100);
    this->addChild(label);

    // finger
    for (int i = 0; i < 2; i++) {
        
        finger[i] = tool.mySprite("finger_print.png");
        finger[i]->setPosition(tool.pos(i == 0 ? 0.25 : 0.75, 0.25));
        finger[i]->setScale(0.5);
        finger[i]->setOpacity(100);
        this->addChild(finger[i]);
    }

    actionLabel(0.0);
    actionFinger(0.0);
    
    this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionLabel), 1.0f);
    this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionFinger), 1.0f);

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TutorialScene::passNotice), 1.0f);
    delayTouch(1.01);

    // touch listener
    touching();
    onKeyReleasing();

    return true;
}

void TutorialScene::actionLabel(float dt) {

    label->runAction(Sequence::create(
        
        FadeTo::create(0.5, 255),
        FadeTo::create(0.5, 100),
        nullptr
    ));
}

void TutorialScene::actionFinger(float dt) {

    for (int i = 0; i < 2; i++) {

        finger[i]->runAction(Sequence::create(
            
            Spawn::create(
                
                ScaleTo::create(0.5, 1.0),
                FadeTo::create(0.5, 255),
                nullptr
            ),
            Spawn::create(
                
                ScaleTo::create(0.5, 0.5),
                FadeTo::create(0.5, 100),
                nullptr
            ),
            nullptr
        ));
    }
}

void TutorialScene::passNotice(float dt) {

    passLabel = tool.myLabel("OK!", tool.getBSz().height * 3/2);
    passLabel->setPosition(tool.ctrPos());
    passLabel->setScale(0.875);
    passLabel->setOpacity(100);
    this->addChild(passLabel);

    passAction(0.0);
    this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::passAction), 1.0f);
}

void TutorialScene::passAction(float dt) {

    passLabel->runAction(Sequence::create(

        Spawn::create (
            
            ScaleTo::create(0.5, 1.0),
            FadeTo::create(0.5, 255),
            nullptr
        ),
        Spawn::create (
            
            ScaleTo::create(0.5, 0.875),
            FadeTo::create(0.5, 100),
            nullptr
        ),
        nullptr
    ));
}

// EVENT LISTENERS //

void TutorialScene::touching() {

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {

        if (isTouchable) {
            
            _eventDispatcher->removeEventListener(touchListener);
            _eventDispatcher->removeEventListener(backButtonListener);
            popGameScene();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void TutorialScene::popGameScene() {

    int recoveredTime = tool.getUDef()->getIntegerForKey("START TIME") + std::time(0) - pausedStartTime;
    tool.setUserValue("START TIME", recoveredTime);

    finger[0]->runAction(RemoveSelf::create());
    finger[1]->runAction(RemoveSelf::create());
    label->runAction(RemoveSelf::create());

    this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionLabel));
    this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionFinger));
    this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::passAction));

    passLabel->runAction(RemoveSelf::create());
    darkScreen->runAction(RemoveSelf::create());
    screenShot_ts->runAction(RemoveSelf::create());

    Director::getInstance()->popScene();
}

void TutorialScene::delayTouch(float dt) {

    isTouchable = false;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TutorialScene::set_isTouchable), dt);
}

void TutorialScene::set_isTouchable(float dt) {isTouchable = true;}

// back button ANDROID
void TutorialScene::onKeyReleasing() {

    backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
                
            if (isTouchable) {
                
                _eventDispatcher->removeEventListener(touchListener);
                _eventDispatcher->removeEventListener(backButtonListener);
                popGameScene();
            }
            tool.playSFX("select on");
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}