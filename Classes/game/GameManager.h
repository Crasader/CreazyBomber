//
//  GameManager.h
//  CreazyBomber
//
//  Created by AIRIA on 14-5-30.
//
//

#ifndef __CreazyBomber__GameManager__
#define __CreazyBomber__GameManager__

#include "common/CommonHeaders.h"
#include "game/objects/Player.h"

class GameManager
{
public:
    static GameManager *getInstance();
    CC_SYNTHESIZE(Player::WalkDirection, m_WalkDirection, WalkDirection);
};

#endif /* defined(__CreazyBomber__GameManager__) */