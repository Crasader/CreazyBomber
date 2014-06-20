//
//  GameScene.cpp
//  CreazyBomber
//
//  Created by 完美计划 on 14-5-25.
//
//

#include "GameScene.h"
#include "game/MapUtil.h"
#include "game/objects/MapObject.h"
#include "components/BomberButton.h"
#include "components/DirectionButton.h"
#include "components/GameUILayer.h"
#include "game/GameManager.h"
#include "game/objects/Bomb.h"
#include "components/MapLayer.h"
#include "components/ResultLayer.h"
#include "components/SettingLayer.h"

void GameScene::onEnter()
{
    BaseLayer::onEnter();
    NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(GameScene::normalBombHandler), ADD_NORMAL_BOMB, NULL);
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameScene::nextLevel), GAME_NEXT, nullptr);
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameScene::retry), GAME_RETRY, nullptr);
    GameManager::getInstance()->setIsGameOver(false);
}

void GameScene::onExit()
{
    BaseLayer::onExit();
    NotificationCenter::getInstance()->removeObserver(this, ADD_NORMAL_BOMB);
    NotificationCenter::getInstance()->removeObserver(this, GAME_NEXT);
}

void GameScene::nextLevel(cocos2d::Ref *pSender)
{
    GameConfig::selectedLevel += 1;
    GameManager::getInstance()->setSpeed(Point::ZERO);
    GameManager::getInstance()->setPlayer(nullptr);
    removeAllChildren();
    GameScene::create()->run();
}

void GameScene::retry(cocos2d::Ref *pSender)
{
    GameManager::getInstance()->setSpeed(Point::ZERO);
    GameManager::getInstance()->setPlayer(nullptr);
    removeAllChildren();
    GameScene::create()->run();
}

bool GameScene::init()
{
    if (!BaseLayer::init()) {
        return false;
    }
    m_fScaleFactor = m_winSize.width/DESIGN_WIDTH;
    auto scaleH = m_winSize.height/DESIGN_HEIGHT;
    if(scaleH>m_fScaleFactor)
    {
        GameManager::getInstance()->setScaleFactor(scaleH);
    }else{
        GameManager::getInstance()->setScaleFactor(m_fScaleFactor);
    }
    
    GameManager::getInstance()->setBombPower(3);
    
    textureFiles.push_back("textures/medium-hd");
    textureFiles.push_back("textures/monster_1-hd");
    textureFiles.push_back("textures/monster_2-hd");
    textureFiles.push_back("textures/scenetex_small-hd");
    textureFiles.push_back("textures/scenetex_medium-hd");
    textureFiles.push_back("textures/scenetex_big-hd");
    textureFiles.push_back("textures/scenetex_big1-hd");
    textureFiles.push_back("textures/player_huxi-hd");
    textureFiles.push_back("textures/ingame-hd");
    textureFiles.push_back("textures/other_1-hd");
    textureFiles.push_back("textures/other-hd");
    textureFiles.push_back("textures/button-hd");
    
    textureFiles.push_back("textures/zh_cn/locale_3-hd");
    textureFiles.push_back("textures/zh_cn/locale_4-hd");
    
    
    char playerTextureName[50];
    sprintf(playerTextureName, "textures/player_%s-hd",GameConfig::selectedRoleName.c_str());
    textureFiles.push_back(playerTextureName);
    
    return true;
}

void GameScene::onTexturesLoaded()
{
    BaseLayer::onTexturesLoaded();
    MapUtil::getInstance()->initMapSize();
    
    auto mapLayer = MapLayer::create();
    auto baseTileLayer = MapUtil::getInstance()->getBaseTileLayer();
    auto tmxLayer = MapUtil::getInstance()->getTmxTileLayer();
    auto commonTileLayer = MapUtil::getInstance()->getCommonTileLayer();
    auto borderLayer = MapUtil::getInstance()->addTileMapBorder();
    
    auto mapSize = MapUtil::getInstance()->getMapSize();
    auto mapSizeInPixle = MapUtil::getInstance()->getMapSizeInPixle();
    mapLayer->setContentSize(mapSizeInPixle);
    
    mapLayer->addChild(baseTileLayer);
    mapLayer->addChild(tmxLayer);
    mapLayer->addChild(borderLayer);
    mapLayer->addChild(commonTileLayer);
    GameManager::getInstance()->setMapTileLayer(commonTileLayer);
    
    mapLayer->setAnchorPoint(Point(0.0f,1.0f));
    mapLayer->setPosition(Point(-TILE_WIDTH/2*m_fScaleFactor,m_winSize.height+TILE_HEIGHT/2*m_fScaleFactor));
    if(mapSize.height==9)
    {
        mapLayer->setAnchorPoint(Point(0.0f,0.5f));
        mapLayer->setPositionY(m_winSize.height/2);
    }
    GameManager::getInstance()->setMonsterCount(MapUtil::getInstance()->getMonsters().size());
    mapLayer->setScale(GameManager::getInstance()->getScaleFactor());
    addChild(mapLayer);
    GameManager::getInstance()->setMapLayer(mapLayer);
    addUIComponents();
    addChild(SettingLayer::getInstance());
    addChild(ResultLayer::create());
    
}

void GameScene::addUIComponents()
{
    auto uiLayer = GameUILayer::create();
    addChild(uiLayer);
    
    auto bombBtn = BomberButton::create();
    bombBtn->setScale(m_fScaleFactor);
    addChild(bombBtn);
    
    auto direc = DirectionButton::create();
    direc->setScale(m_fScaleFactor);
    addChild(direc);
}

void GameScene::normalBombHandler(cocos2d::Ref *pSender)
{
    auto player = GameManager::getInstance()->getPlayer();
    auto coordinate = player->getCoordinate();
    auto tile = MapUtil::getInstance()->getMapObjectFromBombVector(MapUtil::getInstance()->getBomb(), coordinate);
    if(tile&&dynamic_cast<Bomb*>(tile))
    {
        return;
    }
    auto bomb = Bomb::create(Bomb::kBombNormal);
    bomb->setZOrder((coordinate.y)*10+1);
    bomb->setCol(coordinate.x);
    bomb->setRow(coordinate.y);
    bomb->setPower(GameManager::getInstance()->getBombPower());
    bomb->setAnchorPoint(Point(0.5f,0.0f));
    bomb->setPosition(player->convertCoordinate2Point(coordinate));
    
    GameManager::getInstance()->getMapTileLayer()->addChild(bomb);
}
