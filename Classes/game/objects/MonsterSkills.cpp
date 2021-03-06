//
//  MonsterSkills.cpp
//  CreazyBomber
//
//  Created by AIRIA on 14-7-2.
//
//

#include "MonsterSkills.h"
#include "game/MapUtil.h"

bool MonsterFire::init()
{
    if(!Node::init())
    {
        return false;
    }
    setFileName("cl_MBJ_huo.png");
    auto animation = AnimationCache::getInstance()->getAnimation(getFileName());
    if(animation==nullptr)
    {
        Util::addAnimation(getFileName(), 7);
    }
    return true;
}

void MonsterFire::onEnter()
{
    Node::onEnter();
    
    auto mapSizeInPixel = MapUtil::getInstance()->getMapSizeInPixle();
    auto anchor = getAnchorPoint();
    setPosition((getCol()+anchor.x)*TILE_WIDTH, mapSizeInPixel.height- (getRow()+1)*TILE_HEIGHT);
    auto sprite = Sprite::create();
    sprite->setAnchorPoint(Point::ZERO);
    auto animate = Animate::create(AnimationCache::getInstance()->getAnimation(getFileName()));
    sprite->runAction(Sequence::create(animate,CallFunc::create([&]()->void{
        this->removeFromParent();
    }), NULL));
    addChild(sprite);
}