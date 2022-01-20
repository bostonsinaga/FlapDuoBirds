#include "GameOverScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_gos;
int currentScore;

Scene* GameOverScene::createScene(cocos2d::RenderTexture *renTex, int currentScore_in) {

    screenShot_gos = renTex;
    currentScore = currentScore_in;

    auto scene = Scene::create();
    auto layer = GameOverScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool GameOverScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    screenShot_gos->setPosition(tool.ctrPos());
    this->addChild(screenShot_gos);

    auto darkScreen = DrawNode::create();
    darkScreen->drawSolidRect(-Vec2(tool.getVSz() / 2), Vec2(tool.getVSz() / 2), Color4F(0, 0, 0, 127));
    darkScreen->setPosition(tool.ctrPos());
    this->addChild(darkScreen);

    auto board = DrawNode::create();
    board->drawSolidRect(-Vec2(tool.getBSz() * 5), Vec2(tool.getBSz() * 5), Color4F::WHITE);
    board->drawRect(-Vec2(tool.getBSz() * 5), Vec2(tool.getBSz() * 5), Color4F::BLACK);
    board->setPosition(tool.ctrPos());
    this->addChild(board);

    auto title = tool.myLabel("GAME OVER!", tool.getBSz().height * 1.5);
    title->setPosition(Vec2(0, tool.getBSz().height * 3.875));
    title->setColor(Color3B::RED);
    title->enableOutline(Color4B(127, 127, 127, 255), 6);
    board->addChild(title);

    title->runAction(RepeatForever::create(Sequence::create(

        TintTo::create(0.25, Color3B(255, 191, 191)),
        TintTo::create(0.25, Color3B::RED),
        nullptr
    )));

    Label *currentScoreTitle, *currentScoreLabel, *highScoreTitle, *highScoreLabel;

    if (currentScore > tool.getUDef()->getIntegerForKey("HIGH SCORE", 0)) { // new high score

        currentScoreTitle = Label::createWithSystemFont("NEW HIGH SCORE:", "", tool.getBSz().height * 0.75);
        currentScoreTitle->setColor(Color3B(255, 218, 24));
        currentScoreTitle->enableOutline(Color4B::BLACK, 3);

        currentScoreLabel = Label::createWithSystemFont(std::to_string(currentScore), "", tool.getBSz().height * 1.0);
        currentScoreLabel->setColor(Color3B(255, 218, 24));
        currentScoreLabel->enableOutline(Color4B::BLACK, 3);

        highScoreTitle = Label::createWithSystemFont("PREVIOUS HIGH SCORE:", "", tool.getBSz().height * 0.75);
        highScoreTitle->setColor(Color3B::GRAY);
        highScoreTitle->enableOutline(Color4B::BLACK, 3);

        highScoreLabel = Label::createWithSystemFont(std::to_string(tool.getUDef()->getIntegerForKey("HIGH SCORE", 0)), "", tool.getBSz().height * 1.0);
        highScoreLabel->setColor(Color3B::GRAY);
        highScoreLabel->enableOutline(Color4B::BLACK, 3);

        tool.setUserValue("HIGH SCORE", currentScore);
    }
    else { // not reach high score

        currentScoreTitle = Label::createWithSystemFont("CURRENT SCORE:", "", tool.getBSz().height * 0.75);
        currentScoreTitle->setColor(Color3B::GRAY);
        currentScoreTitle->enableOutline(Color4B::BLACK, 3);

        currentScoreLabel = Label::createWithSystemFont(std::to_string(currentScore), "", tool.getBSz().height * 1.0);
        currentScoreLabel->setColor(Color3B::GRAY);
        currentScoreLabel->enableOutline(Color4B::BLACK, 3);

        highScoreTitle = Label::createWithSystemFont("HIGH SCORE:", "", tool.getBSz().height * 0.75);
        highScoreTitle->setColor(Color3B(255, 218, 24));
        highScoreTitle->enableOutline(Color4B::BLACK, 3);

        highScoreLabel = Label::createWithSystemFont(std::to_string(tool.getUDef()->getIntegerForKey("HIGH SCORE", 0)), "", tool.getBSz().height * 1.0);
        highScoreLabel->setColor(Color3B(255, 218, 24));
        highScoreLabel->enableOutline(Color4B::BLACK, 3);
    }

    currentScoreTitle->setPosition(Vec2(0, tool.getBSz().height * 0.75));
    board->addChild(currentScoreTitle);

    currentScoreLabel->setPosition(Vec2(0, tool.getBSz().height * -0.25));
    board->addChild(currentScoreLabel);

    highScoreTitle->setPosition(Vec2(0, tool.getBSz().height * -2.25));
    board->addChild(highScoreTitle);

    highScoreLabel->setPosition(Vec2(0, tool.getBSz().height * -3.25));
    board->addChild(highScoreLabel);

    // schedule
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameOverScene::goToMainMenuScene), 2.0f);

    // listeners
    touching();
    onKeyReleasing();

    return true;
}

// main menu //

void GameOverScene::goToMainMenuScene(float dt) {

    screenShot_gos->runAction(RemoveSelf::create());

    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.25, scene));
}

// EVENT LISTENERS //

void GameOverScene::touching() {

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {
            
        goToMainMenuScene(0.0);
        tool.playSFX("select off");
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// back button ANDROID
void GameOverScene::onKeyReleasing() {

    auto backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
                
            goToMainMenuScene(0.0);
            tool.playSFX("select off");
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}