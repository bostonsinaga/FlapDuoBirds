#include "PauseScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_ps;

Scene* PauseScene::createScene(cocos2d::RenderTexture *renTex) {

    screenShot_ps = renTex;

    auto scene = Scene::create();
    auto layer = PauseScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool PauseScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    // screenshot
    screenShot_ps->setPosition(tool.ctrPos());
    this->addChild(screenShot_ps);

    darkScreen = DrawNode::create();
    darkScreen->drawSolidRect(-Vec2(tool.getVSz() / 2), Vec2(tool.getVSz() / 2), Color4F(0.0, 0.0, 0.0, 0.5));
    darkScreen->setPosition(tool.ctrPos());
    this->addChild(darkScreen);

    // resume button
    resumeButton = DrawNode::create();
    resumeButton->drawSolidRect(

        Vec2(tool.getBSz().width * -1.5, tool.getBSz().height * -1.5),
        Vec2(tool.getBSz().width * -0.5, tool.getBSz().height *  1.5),
        Color4F::WHITE
    );
    resumeButton->drawRect(

        Vec2(tool.getBSz().width * -1.5, tool.getBSz().height * -1.5),
        Vec2(tool.getBSz().width * -0.5, tool.getBSz().height *  1.5),
        Color4F::BLACK
    );
    resumeButton->drawSolidRect(

        Vec2(tool.getBSz().width *  0.5, tool.getBSz().height * -1.5),
        Vec2(tool.getBSz().width *  1.5, tool.getBSz().height * 1.5),
        Color4F::WHITE
    );
    resumeButton->drawRect(

        Vec2(tool.getBSz().width *  0.5, tool.getBSz().height * -1.5),
        Vec2(tool.getBSz().width *  1.5, tool.getBSz().height * 1.5),
        Color4F::BLACK
    );
    resumeButton->setPosition(tool.ctrPos() + Vec2(0, tool.getBSz().height * 3));
    resumeButton->setContentSize(tool.getBSz() * 3);
    this->addChild(resumeButton);

    // home button
    Vec2 hbPoints[3] = {

        Vec2(tool.getBSz().width * -1.5, tool.getBSz().height *  0.0),
        Vec2(tool.getBSz().width *  0.0, tool.getBSz().height *  1.5),
        Vec2(tool.getBSz().width *  1.5, tool.getBSz().height *  0.0)
    };
    homeButton = DrawNode::create();
    homeButton->drawSolidPoly(hbPoints, 3, Color4F::WHITE);
    homeButton->drawPoly(hbPoints, 3, true, Color4F::BLACK);
    homeButton->drawSolidRect(

        Vec2(tool.getBSz().width * -1.5, tool.getBSz().height * -1.5),
        Vec2(tool.getBSz().width *  1.5, tool.getBSz().height * 0.0),
        Color4F::WHITE
    );
    homeButton->drawRect(

        Vec2(tool.getBSz().width * -1.5, tool.getBSz().height * -1.5),
        Vec2(tool.getBSz().width *  1.5, tool.getBSz().height * 0.0),
        Color4F::BLACK
    );
    homeButton->setPosition(tool.ctrPos() - Vec2(0, tool.getBSz().height * 3));
    homeButton->setContentSize(tool.getBSz() * 3);
    this->addChild(homeButton);

    // listeners
    touching();
    onKeyReleasing();

    return true;
}

void PauseScene::touching() {

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {
    
        if (isTouched == false && isTouchable) {

            isTouched = true;
            touchBegan = touch->getLocation();

            if (tool.isAtTouchPos(resumeButton, 'b', touchBegan, touchEnded)) {

                resumeButton->setScale(0.9);
                tool.playSFX("select on");
            }
            else if (tool.isAtTouchPos(homeButton, 'b', touchBegan, touchEnded)) {

                homeButton->setScale(0.9);
                tool.playSFX("select on");
            }
        }

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {
            
        if (isTouchable) {
            
            isTouched = false;
            touchEnded = touch->getLocation();

            resumeButton->setScale(1.0);
            homeButton->setScale(1.0);

            if (tool.isAtTouchPos(resumeButton, 'e', touchBegan, touchEnded)) {

                prepareToPopGameScene();
            }
            else if (tool.isAtTouchPos(homeButton, 'e', touchBegan, touchEnded)) {

                screenShot_ps->runAction(RemoveSelf::create());
                auto scene = GameScene::createScene();
                Director::getInstance()->replaceScene(TransitionFade::create(0.25, scene));
            }

            touchBegan = Vec2(-1, -1);
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// pop game scene //

void PauseScene::prepareToPopGameScene() {

    resumeButton->runAction(RemoveSelf::create());
    homeButton->runAction(RemoveSelf::create());
    darkScreen->runAction(RemoveSelf::create());

    isTouchable = false;

    _eventDispatcher->removeEventListener(touchListener);
    _eventDispatcher->removeEventListener(backButtonListener);

    auto countLabel = Label::createWithSystemFont("3", "", tool.getBSz().height);
    countLabel->setPosition(tool.ctrPos());
    countLabel->enableOutline(Color4B(255, 218, 24, 255), 3);
    this->addChild(countLabel);

    auto effectAct = Spawn::create(
            
        ScaleTo::create(0.75, 1.5),
        Sequence::create(
            
            DelayTime::create(0.375),
            FadeOut::create(0.375),
            nullptr
        ),
        nullptr
    );

    countLabel->runAction(Sequence::create(

        effectAct,
        CallFunc::create([=]() {

            countLabel->setScale(1.0);
            countLabel->setString(std::to_string(2));
            countLabel->setOpacity(255);
        }),
        effectAct,
        CallFunc::create([=]() {

            countLabel->setScale(1.0);
            countLabel->setString(std::to_string(1));
            countLabel->setOpacity(255);
        }),
        effectAct,
        RemoveSelf::create(),
        nullptr
    ));

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&PauseScene::popGameScene), 2.3f);
}

void PauseScene::popGameScene(float dt) {

    int recoveredTime = tool.getUDef()->getIntegerForKey("START TIME") + std::time(0) - pausedStartTime;
    tool.setUserValue("START TIME", recoveredTime);

    screenShot_ps->runAction(RemoveSelf::create());
    Director::getInstance()->popScene();
}

// back button ANDROID
void PauseScene::onKeyReleasing() {

    backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
            
            if (isTouchable) {
                
                prepareToPopGameScene();
                tool.playSFX("select off");
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}