#include "TitleScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene* TitleScene::createScene() {

    auto scene = Scene::create();
    auto layer = TitleScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool TitleScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!LayerGradient::initWithColor(Color4B::WHITE, Color4B(0, 127, 255, 255), Vec2(0, -1))) {

        return false;
    }

    // view //

    auto title = Label::createWithSystemFont("FLAP DUO BIRDS", "", tool.getBSz().height * 2);
    title->setPosition(tool.ctrPos());
    title->enableOutline(Color4B(31, 255, 0, 255), 15);
    title->setDimensions(tool.getBSz().width * 6, tool.getBSz().height * 7);
    title->setHorizontalAlignment(TextHAlignment::CENTER);
    title->setVerticalAlignment(TextVAlignment::CENTER);
    title->setScale(0);
    this->addChild(title);

    title->runAction(Spawn::create(

        Sequence::create(

            RotateBy::create(0.375 ,  22.5),
            RotateBy::create(0.375 , -22.5),
            RotateBy::create(0.375 ,  22.5),
            RotateBy::create(0.375 , -22.5),
            nullptr
        ),
        Sequence::create(
            
            ScaleTo::create(0.375, 0.375, 0.375),
            ScaleTo::create(0.375, 0.000, 0.750),
            ScaleTo::create(0.375, 1.125, 1.125),
            ScaleTo::create(0.375, 1.000, 1.000),
            nullptr
        ),
        nullptr
    ));

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TitleScene::goToGameScene), 2.0f);

    return true;
}

void TitleScene::goToGameScene(float dt) {

    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.25, scene));
}