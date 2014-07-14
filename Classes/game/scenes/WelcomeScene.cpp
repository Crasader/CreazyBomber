//
//  WelcomeScene.cpp
//  CreazyBomber
//
//  Created by AIRIA on 14-7-11.
//
//

#include "WelcomeScene.h"
#include "components/PerfectMenu.h"
#include "game/scenes/GameScene.h"

enum NodeTags{
    kTagWrapperNode,
    kTagNode1,
    kTagNode2,
    kTagNode3,
    kTagNode4
};

bool WelcomeScene::init()
{
    if (!BaseLayer::init()) {
        return false;
    }
    /* 预加载音频文件 */
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic(SOUND_MAIN_BG);
    SimpleAudioEngine::getInstance()->preloadEffect(EFFECT_UI_CLICK);
    SimpleAudioEngine::getInstance()->preloadEffect(EFFECT_UI_ITEM_IN);
    SimpleAudioEngine::getInstance()->preloadEffect(EFFECT_UI_ITEM_OUT);
    
    m_fScaleFactor = m_winSize.width/DESIGN_WIDTH;
    Texture2D::PVRImagesHavePremultipliedAlpha(true);
    /* 预加载的PVR纹理 */
    textureFiles.push_back("textures/bg-hd");
    textureFiles.push_back("textures/opening-hd");
    textureFiles.push_back("textures/zh_cn/locale_1-hd");
    textureFiles.push_back("textures/zh_cn/locale_2-hd");
    textureFiles.push_back("textures/zh_cn/locale_3-hd");
    textureFiles.push_back("textures/zh_cn/locale_4-hd");
    textureFiles.push_back("textures/selectstage-hd");
    textureFiles.push_back("textures/button-hd");
    textureFiles.push_back("textures/medium2-hd");
    textureFiles.push_back("textures/other-hd");
    
    config = GameConfig::getInstance();
    
    return true;
}

ActionInterval *WelcomeScene::getFloatAction(float actTime)
{
    auto moveUp = MoveBy::create(actTime, Point(0,10));
    auto moveDown = MoveBy::create(actTime, Point(0,10));
    float delayTime = rand()%10/10.0f;
    auto seq = Sequence::create(DelayTime::create(delayTime),moveUp,moveUp->reverse(),moveDown,moveDown->reverse(), NULL);
    return RepeatForever::create(seq);
}

void WelcomeScene::_delayCall(float dt,const std::function<void ()> &func)
{
    auto delayTime = DelayTime::create(dt);
    auto delayCall = CallFunc::create(func);
    runAction(Sequence::create(delayTime,delayCall, NULL));
}

void WelcomeScene::_setShieldEnabled(bool val)
{
    log("%s",val?"true":"false");
    getEventDispatcher()->setEnabled(!val);
}

void WelcomeScene::hideElement(cocos2d::ActionInterval *action,CallFunc *callback)
{
    auto hideAct = Sequence::create(action, NULL);
    
    auto hide = [&](Node *parent,CallFunc *callbackHandler=nullptr)->void{
        auto children = parent->getChildren();
        for(auto i=0;i!=children.size();i++)
        {
            auto child = children.at(i);
            auto hideSeq = Sequence::create((ActionInterval*)hideAct->clone(),CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, child)),nullptr);
            /* 如果是menu的话 应该让其中的Item执行Action */
            auto menuNode = dynamic_cast<Menu*>(child);
            if(menuNode)
            {
                auto items = menuNode->getChildren();
                for (auto j=0; j!=items.size(); j++) {
                    if(callbackHandler && j==items.size()-1 && i==children.size()-1)
                    {
                        hideSeq = Sequence::create(hideSeq,callbackHandler, NULL);
                    }
                    items.at(j)->runAction(hideSeq->clone());
                }
            }
            else
            {
                if(callbackHandler&&i==children.size()-1)
                {
                    hideSeq = Sequence::create(hideSeq,callbackHandler, NULL);
                }
                child->runAction(hideSeq);
            }
            
        }
    };
    
    auto wrapper = m_pBody->getChildByTag(kTagWrapperNode);
    for(int i=kTagNode1;i<=kTagNode4;i++)
    {
        if(kTagNode4 == i && callback)
        {
            hide(wrapper->getChildByTag(i),callback);
        }
        else
        {
            hide(wrapper->getChildByTag(i));
        }
    }
}

void WelcomeScene::onTexturesLoaded()
{
    BaseLayer::onTexturesLoaded();
    _initBackground();
}

void WelcomeScene::_initBackground()
{
    /* 背景 */
    auto bg = Sprite::createWithSpriteFrameName("bg.png");
    bg->setPosition(DESIGN_CENTER);
    
    auto bg0 = Sprite::createWithSpriteFrameName("bg0.png");
    bg0->setAnchorPoint(Point::ZERO);
    
    auto bg1 = Sprite::createWithSpriteFrameName("bg1.png");
    bg1->setAnchorPoint(Point::ZERO);
    
    auto bg2 = Sprite::createWithSpriteFrameName("bg2.png");
    bg2->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    bg2->setPosition(DESIGN_RIGHT_BOTTOM);
    /* 云彩 */
    auto cloud = Sprite::createWithSpriteFrameName("cloud.png");
    cloud->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    cloud->setPosition(DESIGN_TOP);
    
    BlendFunc blend = {GL_DST_COLOR, GL_ONE};
    cloud->setBlendFunc(blend);
    
    /* 旋转的阳光 */
    auto sun = Sprite::create("images/sun-hd.png");
    sun->setPosition(Point(300,DESIGN_HEIGHT-60));
    sun->setScale(0.7);
    auto *rotate = RotateBy::create(0.5f, 5);
    sun->runAction(RepeatForever::create(rotate));
    
    /* 包装层 */
    auto wrapper = Node::create();
    wrapper->setTag(kTagWrapperNode);
    wrapper->setPosition(Point(0,-50));
    /**
     * 将这些节点穿插添加到山层之间
     */
    auto createNode = [&](NodeTags tag)->Node*{
        auto target = Node::create();
        target->setTag(tag);
        wrapper->addChild(target);
        return target;
    };
    node1 = createNode(kTagNode1);
    wrapper->addChild(bg0);
    node2 = createNode(kTagNode2);
    wrapper->addChild(bg1);
    node3 = createNode(kTagNode3);
    wrapper->addChild(bg2);
    node4 = createNode(kTagNode4);
    
    bg0->runAction(getFloatAction(3.0f));
    bg1->runAction(getFloatAction(5.0f));
    
    m_pBody->addChild(bg);
    m_pBody->addChild(sun);
    m_pBody->addChild(wrapper);
    m_pBody->addChild(cloud);
    _showWelcome();
}

void WelcomeScene::_showWelcome()
{
    auto createSprite = [&](std::string frameName,Point pos,float scale,Node *parentNode)->Sprite*{
        auto sprite = SPRITE(frameName);
        auto startPos = pos-Point(0,500);
        
        sprite->setScale(scale);
        sprite->setPosition(startPos);
        parentNode->addChild(sprite);
        auto moveTo = MoveTo::create(0.5f, pos);
        auto easeMove = EaseBackOut::create(moveTo);
        float delayTime = rand()%5/10.0f;
        sprite->runAction(Sequence::create(DelayTime::create(delayTime),easeMove,CallFunc::create([&,sprite]()->void{
            sprite->runAction(this->getFloatAction(5.0f));
        }), NULL));
        return sprite;
    };
    
    
    createSprite("lajitong.png",Point(350,180),1.0f,node3);
    createSprite("munaiyi.png",Point(350,400),0.6f,node2);
    createSprite("pinglianren.png",Point(880,290),0.6f,node2);
    createSprite("pig.png",Point(700,450),0.6f,node2);
    createSprite("bianfu.png",Point(670,550),0.6f,node1);
    createSprite("zhizhu.png",Point(700,300),0.9f,node3);
    createSprite("deng.png",Point(200,460),1.0f,node2);
    createSprite("title.png",Point(500,500),0.8f,node4);
    
    /* start game menu */
    auto playBtn = MenuItemSprite::create(SPRITE("play_btn_normal.png"),SPRITE("play_btn_press.png"));
    playBtn->setScale(0.7f);
    playBtn->setPosition(Point(480,320));
    playBtn->setCallback([&](Ref *pSender)->void{
        this->_setShieldEnabled(true);
        Util::playEffect(EFFECT_UI_CLICK);
        Vector<SpriteFrame*> frames;
        for(auto i=1;i<=4;i++)
        {
            auto frameName = __String::createWithFormat("explode%d.png",i)->getCString();
            auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
            frames.pushBack(frame);
        }
        auto animation = Animation::createWithSpriteFrames(frames);
        animation->setDelayPerUnit(0.1f);
        auto explodeAnimate = Animate::create(animation);
        auto target = static_cast<MenuItemSprite*>(pSender);
        auto seq = Sequence::create(explodeAnimate,CallFuncN::create([&](Node *pSender)->void{
            //pSender->getParent()->removeFromParent();
            pSender->setVisible(false);
            /* 移出屏幕 */
            auto moveOut = MoveBy::create(0.5f, Point(0,-600));
            auto easeMove = EaseBackIn::create(moveOut);
            this->hideElement(easeMove,CallFunc::create(CC_CALLBACK_0(WelcomeScene::_showRoles, this)));
        }), NULL);
        target->getNormalImage()->runAction(seq);
    });
    auto start = PerfectMenu::create(playBtn,nullptr);
    start->setPosition(Point::ZERO);
    node4->addChild(start);
}



void WelcomeScene::_showRoles()
{
    log("%s","invoke show roles");
    int i = 0;
    float roleScale = 0.8f;
    
    auto createRoleItem = [&](Role role,std::string normal,std::string press,Point pos)->MenuItemSprite*{
        
        auto item = MenuItemSprite::create(SPRITE(normal), SPRITE(press));
        /* callback lambda  */
        item->setCallback([&,role](Ref *pSender)->void{
            /* 选择完角色后 需要启用屏蔽层 禁止重复操作 */
            this->_setShieldEnabled(true);
            selectRole = role;
            
            switch(role){
                case kRoleSmurf:
                    config->setSelectRoleName("smurf");
                    break;
                case kRoleViking:
                    config->setSelectRoleName("viking");
                    break;
                case kRoleZombie:
                    config->setSelectRoleName("zombie");
                    break;
                case kRoleVampire:
                    config->setSelectRoleName("vampire");
                    break;
                default:
                    break;
            }
            auto menuItem = (MenuItemSprite*)pSender;
            menuItem->selected();
            auto menu = (PerfectMenu*)menuItem->getParent();
            auto children = menu->getChildren();
            auto it = children.begin();
            while (it!=children.end()) {
                if(*it!=menuItem)
                {
                    auto move = MoveTo::create(0.2f,menuItem->getPosition());
                    auto fadeOut = FadeOut::create(0.2f);
                    auto removeSeq = Sequence::create(Spawn::create(move,fadeOut, NULL),
                                                      CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, *it)), NULL);
                    (*it)->runAction(removeSeq);
                }
                it++;
            }
            
            this->_delayCall(0.15f, [this,menuItem]()->void{
                auto flip = OrbitCamera::create(0.6f, 0.5, 0, 0, 360,0, 0);
                menuItem->runAction(flip);
                this->_delayCall(0.9f, [this,menuItem]()->void{
                    auto moveOut = MoveBy::create(0.2f, Point(0,500));
                    menuItem->runAction(Sequence::create(moveOut,CallFuncN::create([this](Node *pSender)->void{
                        pSender->getParent()->removeFromParent();
                        this->_showGameMode();
                    }),nullptr));
                });
            });
            
        });
        item->setPosition(pos);
        item->setScale(roleScale);
        
        
        /* 动画方式进入场景 */
        auto delay = DelayTime::create(i++*0.15f);
        auto moveAct = MoveTo::create(0.4f, pos);
        auto easeInout = EaseBackInOut::create(moveAct);
        /* pos(x,1000)---->pos(x,y) */
        auto beforeRun = CallFuncN::create([](Ref *pSender)->void{
            auto item = (MenuItemSprite*)pSender;
            item->setPosition(Point(item->getPosition().x,1000));
        });
        auto moveSeq = Sequence::create(beforeRun,delay,easeInout,CallFuncN::create([&](Node *pSender)->void{
            pSender->runAction(this->getFloatAction(3.0f));
        }), nullptr);
        item->runAction(moveSeq);
        return item;
    };
    
    
    auto vampire = createRoleItem(kRoleVampire,"selectrole_vampire_normal.png","selectrole_vampire_press.png",Point(160,320));
    auto roleSize = vampire->getContentSize();
    roleSize = Size(roleSize.width*roleScale,roleSize.height*roleScale);
    auto zombie = createRoleItem(kRoleZombie,"selectrole_zombie_normal.png","selectrole_zombie_press.png",vampire->getPosition()+Point(roleSize.width+20,-70));
    auto smurf = createRoleItem(kRoleSmurf,"selectrole_smurf_normal.png","selectrole_smurf_press.png",zombie->getPosition()+Point(roleSize.width+20,130));
    auto viking = createRoleItem(kRoleViking,"selectrole_viking_normal.png","selectrole_viking_press.png",smurf->getPosition()+Point(roleSize.width+20,-80));
    auto selectRole = PerfectMenu::create(vampire,zombie,smurf,viking,nullptr);
    selectRole->setPosition(Point::ZERO);
    m_pBody->addChild(selectRole);

    /* 进入场景完毕之后 禁用屏蔽层 */
    this->_delayCall(i*0.15f+0.4f, [&]()->void{
        this->_setShieldEnabled(false);
    });
}

void WelcomeScene::_hideGameMode()
{
    /* 选择玩游戏模式后 隐藏模式选择的UI */
    log("hide modes");
    Util::playEffect(EFFECT_UI_ITEM_OUT);
}

void WelcomeScene::_showGameMode()
{
    auto actionTime = 0.5f;
    
    auto createMenuItem = [this,actionTime](GameMode mode,std::string normal,std::string press,std::string disable,Point archorPoint,Point pos)->MenuItemSprite*{
        auto easeBackInOut = EaseBackInOut::create(ScaleTo::create(actionTime, 0.85f));
        auto item = MenuItemSprite::create(SPRITE(normal), SPRITE(press),SPRITE(disable));
        item->setAnchorPoint(archorPoint);
        item->setCallback([this,mode,actionTime](Ref *pSender)->void{
            
            if(kGameModeStory == mode)
            {
                
            }
            else if(kGameModeBattle == mode)
            {
                MessageBox("暂未开放,敬请期待~", "提示");
            }
            
            this->_setShieldEnabled(true);
            auto scaleOut = ScaleTo::create(actionTime,0);
            auto easeScale = EaseBackInOut::create(scaleOut);
            this->hideElement(easeScale);
            this->_delayCall(actionTime, [this]()->void{
                this->_showStages();
            });
        });
        item->setPosition(pos);
        item->setScale(0);
        item->setTag(0);
        item->runAction(Sequence::create(easeBackInOut,CallFunc::create([item]()->void{
            auto menu = (Menu*)item->getParent();
            menu->setEnabled(true);
        }), NULL));
        return item;
    };
    
    auto addMenuToNode = [](MenuItemSprite *item,Node *node)->void{
        auto menu = PerfectMenu::create(item,nullptr);
        menu->setPosition(Point::ZERO);
        menu->setEnabled(false);
        menu->setTag(0);
        node->addChild(menu);
    };
    auto storyMode = createMenuItem(kGameModeStory,"story_normal.png","story_press.png","story_press.png",Point(1.0f,0.0f),Point(950,185));
    auto battleMode = createMenuItem(kGameModeBattle,"battle_normal.png","battle_press.png","battle_disable.png",Point::ZERO,Point(30,330));
    battleMode->setEnabled(false);
    
    addMenuToNode(battleMode,node2);
    addMenuToNode(storyMode,node3);
    
    
    this->_delayCall(actionTime, [this]()->void{
        this->_setShieldEnabled(false);
    });
    
    /* change player menu */
    auto changeNormal = SPRITE("change_role.png");
    auto changePress = SPRITE("change_role.png");
    auto changePlayerItem = MenuItemSprite::create(changeNormal, changePress);
    auto changeMenu = PerfectMenu::create(changePlayerItem,nullptr);
    changePlayerItem->setPosition(Point(DESIGN_WIDTH-200,80));
    changeMenu->setPosition(Point::ZERO);
    m_pBody->addChild(changeMenu);
}

void WelcomeScene::_showStages()
{

    auto animateTime = 0.5f;
    auto createItemFunc = [&,animateTime](SceneType scene, std::string normal,std::string press,std::string disable,Point pos)-> MenuItemSprite*{
        
        auto easeBackInOut = EaseBackInOut::create(ScaleTo::create(animateTime, 0.8f));
        auto item = MenuItemSprite::create(SPRITE(normal), SPRITE(press),SPRITE(disable));
        item->setCallback([this,scene,animateTime](Ref *pSender)->void{
            this->_setShieldEnabled(true);
            selectScene = scene;
            auto scaleOut = ScaleTo::create(animateTime,0);
            auto easeScale = EaseBackInOut::create(scaleOut);
            this->hideElement(easeScale);
            this->_delayCall(animateTime, [this]()->void{
                this->_showLevelSelect();
            });
        });
        //        item->setEnabled(false);
        item->setPosition(pos);
        item->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        item->setScale(0);
        item->runAction(easeBackInOut);
        
        return item;
    };
    
    auto cl_story = createItemFunc(kSceneCL_Story,"clstorynormal.png", "clstorypress.png", "clstorydisable.png",Point(300,10));
    auto md_story = createItemFunc(kSceneMD_Story,"mdstorynormal.png", "mdstorypress.png", "mdstorydisable.png",Point(550,100));
    auto bc_story = createItemFunc(kSceneBC_Story,"bcstorynormal.png", "bcstorypress.png", "bcstorydisable.png",Point(800,100));
    cl_story->setEnabled(__userDefault->getBoolForKey(KEY_CL));
    md_story->setEnabled(__userDefault->getBoolForKey(KEY_MD));
    bc_story->setEnabled(__userDefault->getBoolForKey(KEY_BC));
    
    auto cl_battle = createItemFunc(kSceneCL_Battle,"clbattlenormal.png", "clbattlepress.png", "clbattledisable.png",Point(700,230));
    auto md_battle = createItemFunc(kSceneMD_Battle,"mdbattlenormal.png", "mdbattlepress.png", "mdbattledisable.png",Point(450,250));
    auto bc_battle = createItemFunc(kSceneBC_Battle,"bcbattlenormal.png", "bcbattlepress.png", "bcbattledisable.png",Point(200,260));
    cl_battle->setEnabled(__userDefault->getBoolForKey(KEY_CL_BATTLE));
    md_battle->setEnabled(__userDefault->getBoolForKey(KEY_MD_BATTLE));
    bc_battle->setEnabled(__userDefault->getBoolForKey(KEY_BC_BATTLE));
    
    auto addMenu = [this,animateTime](MenuItemSprite *cl,MenuItemSprite *md,MenuItemSprite *bc,Node *node)->void{
        auto menu = PerfectMenu::create(cl,md,bc,nullptr);
        menu->setPosition(Point::ZERO);
        //        menu->setEnabled(false);
        node->addChild(menu);
        this->_delayCall(animateTime, [this]()->void{
            this->_setShieldEnabled(false);
        });
    };
    
    addMenu(cl_story,md_story,bc_story,node3);
    addMenu(cl_battle,md_battle,bc_battle,node2);
    cl_story->setEnabled(true);
}

void WelcomeScene::_showLevelSelect()
{
    std::string type = "bc";
    switch (selectScene) {
        case kSceneCL_Story:
            type = "cl";
            break;
        case kSceneBC_Story:
            type = "bc";
            break;
        case kSceneMD_Story:
            type = "md";
            break;
        case kSceneCL_Battle:
            type = "cl_battle";
            break;
        case kSceneMD_Battle:
            type = "md_battle";
            break;
        case kSceneBC_Battle:
            type = "bc_battle";
        default:
            break;
    }
    config->setSelectSceneName(type);
    
    
    std::vector<Point> points = {Point(350,70),Point(510,140),Point(670,180),Point(830,250),Point(640,300),Point(440,330),Point(270,340),Point(100,330),Point(400,450),Point(560,460),Point(720,450),Point(870,430)};
    
    if(type.find("battle")!=std::string::npos)
    {
        points = {Point(350,70),Point(510,140),Point(670,180),Point(830,250),Point(640,300),Point(440,330),Point(270,340),Point(100,330)};
    }
    
    auto createMenu = [](Node *parentNode)->PerfectMenu*{
        auto menu = PerfectMenu::create();
        parentNode->addChild(menu);
        menu->setPosition(Point::ZERO);
        return menu;
    };
    
    /* 获取到达的关卡信息 */
    int level = __userDefault->getIntegerForKey(__String::createWithFormat("key_%s_level",GameConfig::getInstance()->getSelectSceneName().c_str())->getCString());
    
    type = type.substr(0,2);
    
    int idx = 0;
    auto it = points.begin();
    
    auto row1 = createMenu(node1);
    auto row2 = createMenu(node2);
    auto row3 = createMenu(node3);
    PerfectMenu *menu = nullptr;
    MenuItemSprite *stage = nullptr;
    auto showAct = ScaleTo::create(0.2, 0.8f);
    auto easeBackIn = EaseBackInOut::create(showAct);
    
    auto getMapName = [](int idx)->std::string{
        /* 获取地图名称 */
        char mapName[20];
        int prefix = 1100;
        auto config = GameConfig::getInstance();
        auto name = config->getSelectSceneName();
        if (name=="cl") {
            prefix = 1100;
        }else if(name=="md"){
            prefix = 1200;
        }else if(name=="bc"){
            prefix = 1300;
        }else if(name=="cl_battle"){
            prefix = 2100;
        }else if(name=="md_battle"){
            prefix = 2200;
        }else if(name=="bc_battle"){
            prefix = 2300;
        }
        prefix += idx;
        sprintf(mapName, "%d",prefix+1);
        return mapName;
    };
    while (it!=points.end()) {
        auto key = __String::createWithFormat("%s_rate",getMapName(idx).c_str())->getCString();
        auto rate = __userDefault->getIntegerForKey(key,0);
        auto fileName = __String::createWithFormat("%s_stage1_star%d.png",GameConfig::getInstance()->getSelectSceneName().substr(0,2).c_str(),rate)->getCString();
        if(idx<3)
        {
            
            menu = row3;
            stage = MenuItemSprite::create(SPRITE(type+"_stage1_normal.png"), SPRITE(type+"_stage1_press.png"),SPRITE(type+"_stage1_disable.png"));
        }
        else if(idx<8)
        {
            fileName = __String::createWithFormat("%s_stage2_star%d.png",GameConfig::getInstance()->getSelectSceneName().substr(0,2).c_str(),rate)->getCString();
            menu = row2;
            stage = MenuItemSprite::create(SPRITE(type+"_stage2_normal.png"), SPRITE(type+"_stage2_press.png"),SPRITE(type+"_stage2_disable.png"));
        }
        else
        {
            menu = row1;
            stage = MenuItemSprite::create(SPRITE(type+"_stage1_normal.png"), SPRITE(type+"_stage1_press.png"),SPRITE(type+"_stage1_disable.png"));
        }
        stage->setUserData(new int(idx));
        stage->setPosition(*it);
        stage->setAnchorPoint(Point(0.5,0));
        stage->setScale(0.0f);
        stage->runAction(this->getFloatAction(3.0f));
        if(idx<level){
            stage->setEnabled(true);
            /* 设置评级 */
            if(rate>0)
            {
                auto star = SPRITE(fileName);
                star->setPosition(0,110);
                star->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                stage->addChild(star);
            }
        }
        else
        {
            stage->setEnabled(false);
        }
        auto showSeq = Sequence::create(DelayTime::create(idx*0.15f),easeBackIn->clone(), NULL);
        stage->runAction(showSeq);
        stage->setCallback([this](Ref *pSender)->void{
            this->_setShieldEnabled(true);
            auto item = (MenuItemSprite*)pSender;
            void *userData = item->getUserData();
            int idx = *static_cast<int*>(userData);
            GameConfig::getInstance()->setSelectLevel(idx+1);
            auto scaleOut = ScaleTo::create(0.3f, 0);
            auto easeScale = EaseBackIn::create(scaleOut);
            this->hideElement(easeScale);
            this->_delayCall(0.4f, [this]()->void{
                this->_setShieldEnabled(false);
                GameScene::create()->run();
            });
        });
        menu->addChild(stage);
        it++;
        idx++;
    }
    this->_setShieldEnabled(false);
}