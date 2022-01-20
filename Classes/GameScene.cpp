#include "GameScene.h"
#include "GameOverScene.h"
#include "PauseScene.h"
#include "TutorialScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {

    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2::ZERO);
    // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); // debug only

    auto layer = GameScene::create();
    layer->setPhysicsWorld(scene->getPhysicsWorld());
    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!LayerGradient::initWithColor(Color4B::WHITE, Color4B(0, 127, 255, 255), Vec2(0, -1))) {

        return false;
    }

    //// inits ////

    auto edge = Node::create();
    auto edgeBody = PhysicsBody::createEdgeBox(tool.getVSz());
    edgeBody->setDynamic(false);
    edgeBody->setContactTestBitmask(true);
    edgeBody->setCollisionBitmask(obs_colMask);
    edge->setPhysicsBody(edgeBody);
    edge->setPosition(tool.ctrPos());
    this->addChild(edge);

    Vec2 polDots_bird[3] = {

        Vec2(tool.getBSz().width / 2, 0),
        Vec2(tool.getBSz().width / 2, tool.getBSz().height / 4),
        Vec2(tool.getBSz().width, tool.getBSz().height / 8)
    };

    Vec2 polDots_wing[3][4] = {

        {
            Vec2(-tool.getBSz().width / 4, -tool.getBSz().height / 16),
            Vec2(-tool.getBSz().width / 4,  tool.getBSz().height / 16),
            Vec2( tool.getBSz().width / 4,  tool.getBSz().height / 16),
            Vec2( tool.getBSz().width / 4, -tool.getBSz().height / 16)
        },
        {
            Vec2(-tool.getBSz().width / 4, -tool.getBSz().height / 8),
            Vec2(-tool.getBSz().width / 4,  tool.getBSz().height / 12),
            Vec2( tool.getBSz().width / 4,  tool.getBSz().height / 12),
            Vec2( tool.getBSz().width / 4, -tool.getBSz().height / 12)
        },
        {
            Vec2(-tool.getBSz().width / 4, -tool.getBSz().height / 4),
            Vec2(-tool.getBSz().width / 4,  tool.getBSz().height / 8),
            Vec2( tool.getBSz().width / 4,  tool.getBSz().height / 8),
            Vec2( tool.getBSz().width / 4, -tool.getBSz().height / 6)
        }
    };

    for (int i = 0; i < 2; i++) {

        bird[i] = DrawNode::create();
        redrawBird(i);
        generateBirdBody(i);
        bird[i]->drawSolidPoly(polDots_bird, 3, Color4F::YELLOW);
        bird[i]->setPosition(tool.ctrPos() + Vec2(tool.getBSz().width * (i == 0 ? -2 : 2), 0));
        bird[i]->setRotation(-5.625);
        bird[i]->setTag(i);
        this->addChild(bird[i], 2);

        for (int j = 0 ; j < 3; j++) {
            
            birdWing[i][j] = DrawNode::create();
            birdWing[i][j]->drawSolidPoly(polDots_wing[j], 4, Color4F::YELLOW);
            birdWing[i][j]->setPosition(Point(
                
                -tool.getBSz().width / 4,
                (j == 0 ? -tool.getBSz().height * 1/4  :
                (j == 1 ? -tool.getBSz().height * 7/24 : -tool.getBSz().height * 3/8))
            ));
            if (j != 2) birdWing[i][j]->setOpacity(0);
            bird[i]->addChild(birdWing[i][j]);
        }
    }

    // carnage
    for (int i = 0; i < 2; i++) tool.mySpriteFrames(carnage[i], 6, "GRAPHICS/carnage_%d.png", 3, this);

    // count label
    countLabel = Label::createWithSystemFont("", "", tool.getBSz().width);
    countLabel->setPosition(tool.pos(0.5, 1.0) - Vec2(0, tool.getBSz().height * 2));
    countLabel->enableOutline(Color4B(0, 127, 255, 255), 5);
    this->addChild(countLabel, 10);

    // score bar //

    scoreBar.bg = DrawNode::create();
    this->addChild(scoreBar.bg, 10);

    scoreBar.lb = Label::createWithSystemFont("", "", tool.getBSz().height);
    scoreBar.lb->enableOutline(Color4B::BLACK, 3);
    scoreBar.lb->setColor(Color3B::GRAY);
    scoreBar.bg->addChild(scoreBar.lb);

    scoreMainMenu();

    // play button
    Vec2 pbPoints[3] = {

        -Vec2(tool.getBSz().width * 1.5, tool.getBSz().height * 1.5),
        Vec2(-tool.getBSz().width * 1.5, tool.getBSz().height * 1.5),
        Vec2(tool.getBSz().width * 1.5, 0)
    };
    playButton = DrawNode::create();
    playButton->drawSolidPoly(pbPoints, 3, Color4F::WHITE);
    playButton->drawPoly(pbPoints, 3, true, Color4F::BLACK);
    playButton->setContentSize(tool.getBSz() * 3);
    playButton->setPosition(tool.pos(0.5, 0.25));
    this->addChild(playButton, 10);

    // listeners
    touching();
    contacting();
    onKeyReleasing();

    // schedules
    birdFlaps_schedule(0.0);
    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::birdFlaps_schedule), flapTime * 2);
    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::buildingSpawn), float(54.0f / 24.0f));
    
    return true;
}

// bird //

void GameScene::birdFalls_1(float dt) {birdBody[0]->setVelocity(Vec2(0, -333));}
void GameScene::birdFalls_2(float dt) {birdBody[1]->setVelocity(Vec2(0, -333));}

void GameScene::birdFlaps(int tag) {

    bird[tag]->runAction(Sequence::create(

        RotateTo::create(flapTime / 3, 5.625),
        RotateTo::create(flapTime / 3, 0.0),
        RotateTo::create(flapTime / 3, -5.625),
        nullptr
    ));

    birdWing[tag][0]->runAction(Sequence::create(

        DelayTime::create(flapTime / 3),
        FadeIn::create(0),
        DelayTime::create(flapTime / 3),
        FadeOut::create(0),
        nullptr
    ));

    birdWing[tag][1]->runAction(Sequence::create(

        FadeIn::create(0),
        DelayTime::create(flapTime / 3),
        FadeOut::create(0),
        DelayTime::create(flapTime / 3),
        FadeIn::create(0),
        DelayTime::create(flapTime / 3),
        FadeOut::create(0),
        nullptr
    ));

    birdWing[tag][2]->runAction(Sequence::create(

        FadeOut::create(0),
        DelayTime::create(flapTime),
        FadeIn::create(0),
        nullptr
    ));
}

void GameScene::birdFlaps_schedule(float dt) {
    
    for (int i = 0; i < 2; i++) birdFlaps(i);
    tool.playSFX("flapping");
}

void GameScene::atCollideBird(int tag) {

    if (isShow[tag]) {
        
        fallenBirdsCount++;

        if (fallenBirdsCount == 1) {

            if (tag == 0) {
                
                respawnCountdown_1(0.0);
                this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::respawnCountdown_1), 1.0f);
            }
            else {

                respawnCountdown_2(0.0);
                this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::respawnCountdown_2), 1.0f);
            }
        }
        else if (fallenBirdsCount >= 2) {

            countLabel->setString("");
            this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::birdFlaps_schedule));
            this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::respawnCountdown_1));
            this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::respawnCountdown_2));
            this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::goToGameScene), 1.0f);
        }

        isShow[tag] = false;
        birdBody[tag]->setVelocity(Vec2::ZERO);

        tool.mySpriteAnimation(carnage[tag], 6, 0.05, bird[tag]->getPosition());
        bird[tag]->setScale(0);
        bird[tag]->setPosition(tool.pos(0.5, 2.0) + Vec2(tool.getBSz().width * (tag == 0 ? -2 : 2), 0));

        tool.playSFX("hit");
    }
}

void GameScene::redrawBird(int tag) {

    bird[tag]->drawSolidRect(-Vec2(tool.getBSz() / 2), Vec2(tool.getBSz() / 2), birdColors[std::rand() % 10]);
    bird[tag]->drawSolidCircle(tool.getBSz() / 4, tool.getBSz().width / 16, 0.0, 100, Color4F::BLACK);
}

void GameScene::generateBirdBody(int tag) {

    if (birdBody[tag] != nullptr) birdBody[tag]->removeFromWorld();
    birdBody[tag] = PhysicsBody::createBox(tool.getBSz());
    birdBody[tag]->setDynamic(false);
    birdBody[tag]->setContactTestBitmask(true);
    birdBody[tag]->setCollisionBitmask(bird_colMask[tag]);
    bird[tag]->setPhysicsBody(birdBody[tag]);
}

// obstacle //

void GameScene::obsSpawn(float dt) {

    std::srand(std::time(0));

    int
        deltaStartTime = std::time(0) - tool.getUDef()->getIntegerForKey("START TIME"),
        randInt = std::rand() % 11 - 5;

    if (deltaStartTime <= 30) obsSpawnTimeDistance = 4.0;
    else if (deltaStartTime <= 60) obsSpawnTimeDistance = 3.5;
    else obsSpawnTimeDistance = 3.0;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::obsSpawn), obsSpawnTimeDistance);

    float gapHeightRate = randInt * tool.getBSz().height;

    Vec2 obsPoint_up = Vec2(
        
        tool.pos(1.0, 0.5).x + tool.getBSz().width,
        (tool.ctrPos().y + gapHeightRate + tool.getVSz().height) / 2
    );

    Size obsSize_up = Size(
        
        tool.getBSz().width * 2,
        tool.getVSz().height / 2 - gapHeightRate
    );

    Vec2 obsPoint_down = Vec2(
        
        tool.pos(1.0, 0.5).x + tool.getBSz().width,
        (obsPoint_up.y - obsSize_up.height / 2 - tool.getBSz().height * 5) / 2
    );

    Size obsSize_down = Size(
        
        tool.getBSz().width * 2,
        obsPoint_down.y * 2
    );

    DrawNode *obs[2] = {DrawNode::create(), DrawNode::create()};
    PhysicsBody *obsBody[2];

    Node *node[2];
    PhysicsBody *nodeBody[2];

    Node *gap;
    PhysicsBody *gapBody;

    obs[0]->drawSolidRect(-Vec2(obsSize_up / 2), Vec2(obsSize_up / 2), Color4F(0.125, 0.875, 0.0, 1.0));
    obs[0]->drawSolidRect(
        
        -Vec2(obsSize_up.width / 2 + tool.getBSz().width / 2, obsSize_up.height / 2),
        Vec2(obsSize_up.width / 2 + tool.getBSz().width / 2, -obsSize_up.height / 2 + tool.getBSz().height),
        Color4F(0.125, 1.0, 0.0, 1.0)
    );
    obsBody[0] = PhysicsBody::createBox(obsSize_up);
    obsBody[0]->setDynamic(false);
    obsBody[0]->setContactTestBitmask(true);
    obsBody[0]->setCollisionBitmask(obs_colMask);
    obs[0]->setPhysicsBody(obsBody[0]);
    obs[0]->setPosition(obsPoint_up);
    this->addChild(obs[0], 1);

    node[0] = Node::create();
    nodeBody[0] = PhysicsBody::createEdgeBox(Size(tool.getBSz().width * 3, tool.getBSz().height));
    nodeBody[0]->setDynamic(false);
    nodeBody[0]->setContactTestBitmask(true);
    nodeBody[0]->setCollisionBitmask(obs_colMask);
    node[0]->setPhysicsBody(nodeBody[0]);
    node[0]->setPosition(obsPoint_up - Vec2(0, obsSize_up.height / 2 - tool.getBSz().height / 2));
    this->addChild(node[0], 1);

    obs[1]->drawSolidRect(-Vec2(obsSize_down / 2), Vec2(obsSize_down / 2), Color4F(0.125, 0.875, 0.0, 1.0));
    obs[1]->drawSolidRect(
        
        -Vec2(obsSize_down.width / 2 + tool.getBSz().width / 2, -obsSize_down.height / 2 + tool.getBSz().height),
        Vec2(obsSize_down.width / 2 + tool.getBSz().width / 2, obsSize_down.height / 2),
        Color4F(0.125, 1.0, 0.0, 1.0)
    );
    obsBody[1] = PhysicsBody::createBox(obsSize_down);
    obsBody[1]->setDynamic(false);
    obsBody[1]->setContactTestBitmask(true);
    obsBody[1]->setCollisionBitmask(obs_colMask);
    obs[1]->setPhysicsBody(obsBody[1]);
    obs[1]->setPosition(obsPoint_down);
    this->addChild(obs[1], 1);

    node[1] = Node::create();
    nodeBody[1] = PhysicsBody::createEdgeBox(Size(tool.getBSz().width * 3, tool.getBSz().height));
    nodeBody[1]->setDynamic(false);
    nodeBody[1]->setContactTestBitmask(true);
    nodeBody[1]->setCollisionBitmask(obs_colMask);
    node[1]->setPhysicsBody(nodeBody[1]);
    node[1]->setPosition(obsPoint_down + Vec2(0, obsSize_down.height / 2 - tool.getBSz().height / 2));
    this->addChild(node[1], 1);

    gap = Node::create();
    gapBody = PhysicsBody::createBox(Size(obsSize_up.width / 4, tool.getVSz().height - obsSize_up.height - obsSize_down.height));
    gapBody->setDynamic(false);
    gapBody->setContactTestBitmask(true);
    gapBody->setCollisionBitmask(gap_colMask);
    gap->setPhysicsBody(gapBody);
    gap->setPosition((obsPoint_up - Vec2(0, obsSize_up.height / 2) + obsPoint_down + Vec2(0, obsSize_down.height / 2)) / 2);
    this->addChild(gap, 1);

    gapBody->setVelocity(Vec2(-200, 0));
    gap->runAction(Sequence::create(

        DelayTime::create(10),
        CallFunc::create([=]() {gapBody->removeFromWorld();}),
        RemoveSelf::create(),
        nullptr
    ));

    // actions
    for (int i = 0; i < 2; i++) {

        obsBody[i]->setVelocity(Vec2(-200, 0));
        nodeBody[i]->setVelocity(Vec2(-200, 0));
        
        obs[i]->runAction(Sequence::create(

            DelayTime::create(10),
            CallFunc::create([=]() {obsBody[i]->removeFromWorld();}),
            RemoveSelf::create(),
            nullptr
        ));

        node[i]->runAction(Sequence::create(

            DelayTime::create(10),
            CallFunc::create([=]() {nodeBody[i]->removeFromWorld();}),
            RemoveSelf::create(),
            nullptr
        ));
    }
}

void GameScene::buildingSpawn(float dt) {

    std::srand(std::time(0));
    int randVal = std::rand() % 12 + 1;
    if (randVal < 3) randVal = 3;

    Vec2 pos[3 * randVal];
    int h = 0;
    float decreaseBSz_H = randVal % 2 == 0 ? tool.getBSz().height * 0.5 : 0.0;

    for (int i = -randVal / 2; i <= randVal / 2; i++) {

        int valSign = i < 0 ? 1 : -1;

        if (randVal % 2 == 0 && i == 0) i++;
    
        for (int j = -1; j <= 1; j++) {
        
            pos[h] = Vec2(j * tool.getBSz().width, i * tool.getBSz().height + valSign * decreaseBSz_H);
            h++;
        }
    }

    Size buildingSize = Size(tool.getBSz().width * 3, tool.getBSz().height * randVal);

    auto building = DrawNode::create();
    building->drawSolidRect(-Vec2(buildingSize) / 2, Vec2(buildingSize) / 2, Color4F(0.875, 0.875, 0.875, 1.0));
    
    for (int i = 0; i < 3 * randVal; i++) {

        building->drawSolidRect(
            
            pos[i] - tool.getBSz() / 4,
            pos[i] + tool.getBSz() / 4,
            Color4F(0.0, 0.65, 1.0, 1.0)
        );
    }

    building->setPosition(tool.pos(1.0, 0.0) + Vec2(buildingSize / 2));
    building->runAction(Sequence::create(
    
        MoveBy::create(18, -Vec2(tool.getBSz().width * 24, 0)),
        RemoveSelf::create(),
        nullptr
    ));
    this->addChild(building);
}

// gui //

void GameScene::respawnCountdown_1(float dt) {respawnCountdown(0);}
void GameScene::respawnCountdown_2(float dt) {respawnCountdown(1);}

void GameScene::respawnCountdown(int tag) {

    countLabel->setString(std::to_string(countDown));

    if (countDown <= 0) {

        countDown = 6;
        countLabel->setString("");
        
        tag == 0 ?
            this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::respawnCountdown_1)):
            this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::respawnCountdown_2));

        isShow[tag] = true;

        redrawBird(tag);

        bird[tag]->setPosition(tool.ctrPos() + Vec2(tool.getBSz().width * (tag == 0 ? -2 : 2), 0));
        bird[tag]->runAction(Sequence::create(
            
            ScaleTo::create(0.25, 1.0),
            CallFunc::create([=]() {
                
                generateBirdBody(tag);
                tag == 0 ? birdFalls_1(0.0) : birdFalls_2(0.0);
            }),
            nullptr
        ));

        fallenBirdsCount--;
    }
    
    countDown--;
}

void GameScene::scoreMainMenu() {

    int highScore = tool.getUDef()->getIntegerForKey("HIGH SCORE", 0);
    Size size = Size(getScoreDigitCount(highScore) * tool.getBSz().width / 2 + tool.getBSz().width, tool.getBSz().height);

    scoreBar.bg->drawSolidRect(-Vec2(size / 2), Vec2(size / 2), Color4F::WHITE);
    scoreBar.bg->drawRect(-Vec2(size / 2), Vec2(size / 2), Color4F::BLACK);
    scoreBar.bg->setPosition(tool.pos(1.0, 1.0) - size / 2 - tool.getBSz() / 2);

    scoreBar.lb->setString(std::to_string(highScore));

    generateTitleScore(size, true);
}

void GameScene::generateTitleScore(cocos2d::Size scoreBarSize, bool isMainMenu) {

    Size tlbgsz = Size(tool.getBSz().width * (isMainMenu == true ? 3.5 : 4.5), tool.getBSz().height / 2);

    scoreBar.tlbg = DrawNode::create();
    scoreBar.tlbg->drawSolidRect(-Vec2(tlbgsz / 2), Vec2(tlbgsz / 2), Color4F::WHITE);
    scoreBar.tlbg->drawRect(-Vec2(tlbgsz / 2), Vec2(tlbgsz / 2), Color4F::BLACK);
    scoreBar.tlbg->setPosition(scoreBar.bg->getPosition() + Vec2(-scoreBarSize.width / 2 - tlbgsz.width / 2, scoreBarSize.height * 0.25));
    this->addChild(scoreBar.tlbg, 10);

    scoreBar.tl = Label::createWithSystemFont(isMainMenu == true ? "HIGH SCORE" : "NEW HIGH SCORE", "", tool.getBSz().height / 2);
    scoreBar.tl->setPosition(scoreBar.tlbg->getPosition());
    scoreBar.tl->enableOutline(Color4B::BLACK, 3);
    scoreBar.tl->setColor(Color3B::GRAY);
    this->addChild(scoreBar.tl, 10);
}

void GameScene::removeScoreTitle(float dt) {

    if (isCelebrating) {
    
        isCelebrating = false;
        scoreBar.cllb->runAction(RemoveSelf::create());
    }

    scoreBar.tl->runAction(RemoveSelf::create());
    scoreBar.tlbg->runAction(RemoveSelf::create());
}

void GameScene::scoreUp(int tag, bool isAction) {

    if (isShow[tag]) {
        
        currentScore++;
        Size size = Size(getScoreDigitCount(currentScore) * tool.getBSz().width / 2 + tool.getBSz().width, tool.getBSz().height);

        if (isCelebrating) scoreBar.cllb->setVisible(false);

        scoreBar.lb->retain();
        scoreBar.lb->removeFromParent();

        scoreBar.bg->runAction(RemoveSelf::create());
        scoreBar.bg = DrawNode::create();
        scoreBar.bg->drawSolidRect(-Vec2(size / 2), Vec2(size / 2), Color4F::WHITE);
        scoreBar.bg->drawRect(-Vec2(size / 2), Vec2(size / 2), Color4F::BLACK);
        scoreBar.bg->setPosition(tool.pos(1.0, 1.0) - size / 2 - tool.getBSz() / 2);
        this->addChild(scoreBar.bg, 10);

        scoreBar.bg->addChild(scoreBar.lb);
        scoreBar.lb->release();

        scoreBar.lb->setString(std::to_string(currentScore));

        if (currentScore > tool.getUDef()->getIntegerForKey("HIGH SCORE", 0) && isUncelebrated) {

            isCelebrating = true;
            isUncelebrated = false;

            scoreBar.cllb = Label::createWithSystemFont(scoreBar.lb->getString(), "", tool.getBSz().height);
            scoreBar.cllb->setPosition(scoreBar.bg->getPosition());
            scoreBar.cllb->enableOutline(Color4B::BLACK, 3);
            scoreBar.cllb->setColor(Color3B(255, 218, 24));
            this->addChild(scoreBar.cllb, 11);

            generateTitleScore(size, false);
            this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::removeScoreTitle), 2.0f);
            tool.playSFX("new high score");
        }

        if (isAction) tool.playSFX("score");
    }
}

int GameScene::getScoreDigitCount(int score) {

    int digitCount = 0;

    for (int i = 10; i <= 1000000000; i *= 10) {

        if (score > 0) {
            
            score /= i;
            digitCount++;
            if (score == 0) break;
        }
        else digitCount = 1;
    }

    return digitCount;
}

// scenes //

void GameScene::startGame() {

    isGameStarted = true;
    tool.setUserValue("START TIME", std::time(0));

    playButton->runAction(RemoveSelf::create());
    removeScoreTitle(0.0);
    scoreUp(0, false);

    birdFalls_1(0.0);
    birdFalls_2(0.0);

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::obsSpawn), 1.0f);
    checkingTutorialNeeds();
}

void GameScene::checkingTutorialNeeds() {

    if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED", true))
        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut1), 0.5f);
}

void GameScene::tut_tut1(float dt) {

    auto scene = TutorialScene::createScene(getRenTex());
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());
    
    tool.setUserValue("IS TUTORIAL NEEDED", false);
}

void GameScene::goToGameScene(float dt) {

    auto scene = GameOverScene::createScene(getRenTex(), currentScore);
    Director::getInstance()->replaceScene(TransitionFade::create(0.0, scene));
}

cocos2d::RenderTexture *GameScene::getRenTex() {

    auto renTex = RenderTexture::create(tool.getVSz().width, tool.getVSz().height);
    renTex->setPosition(tool.ctrPos());

    renTex->begin();
    this->getParent()->visit();
    renTex->end();

    return renTex;
}

//// EVENT LISTENERS ////

// touch event
void GameScene::touching() {

    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [=](const std::vector<Touch*> &touches, Event *event) {

        touchBegan = touches.back()->getLocation();

        if (isGameStarted) {

            int tag = touchBegan.x < tool.ctrPos().x ? 0 : 1;

            if (isShow[tag]) {
                
                birdBody[tag]->setVelocity(Vec2::ZERO);
                bird[tag]->runAction(MoveTo::create(flapTime, bird[tag]->getPosition() + Vec2(0, tool.getBSz().height * 2)));
                tool.playSFX(tag == 0 ? "flap 1" : "flap 2");

                if      (tag == 0) this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::birdFalls_1), flapTime);
                else if (tag == 1) this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::birdFalls_2), flapTime);
            }
        }
        else {
            
            if (tool.isAtTouchPos(playButton, 'b', touchBegan, touchEnded)) {

                playButton->setScale(0.9);
                tool.playSFX("select on");
            }
        }

        return true;
    };
    touchListener->onTouchesEnded = [=](const std::vector<Touch*> &touches, Event *event) {

        touchEnded = touches.back()->getLocation();

        if (isGameStarted == false) {

            playButton->setScale(1.0);
            
            if (tool.isAtTouchPos(playButton, 'e', touchBegan, touchEnded)) {

                startGame();
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// physics contact //

void GameScene::contacting() {

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = [=](PhysicsContact &contact) {

        PhysicsBody *a = contact.getShapeA()->getBody();
        PhysicsBody *b = contact.getShapeB()->getBody();

        // checks whether are bodies have collided
        if (tool.isAtContact(bird_colMask[0], obs_colMask, a, b)) {

            atCollideBird(0);
        }
        else if (tool.isAtContact(bird_colMask[1], obs_colMask, a, b)) {

            atCollideBird(1);
        }
        else if (tool.isAtContact(bird_colMask[0], gap_colMask, a, b)) {

            scoreUp(0, true);
        }
        else if (tool.isAtContact(bird_colMask[1], gap_colMask, a, b)) {

            scoreUp(1, true);
        }

        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// back button ANDROID
void GameScene::onKeyReleasing() {

    auto backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {

            tool.playSFX("select off");

            if (isGameStarted) {
                
                auto scene = PauseScene::createScene(getRenTex());
                Director::getInstance()->pushScene(scene);
            }
            else {

                Director::getInstance()->end();
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}