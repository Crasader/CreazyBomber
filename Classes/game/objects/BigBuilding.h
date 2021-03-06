//
//  BigBuilding.h
//  CreazyBomber
//
//  Created by AIRIA on 14-5-27.
//
//

#ifndef __CreazyBomber__BigBuilding__
#define __CreazyBomber__BigBuilding__

#include "game/objects/MapObject.h"
#include "game/objects/MapCell.h"

class BigBuilding:public MapObject
{
public:
    MAPOBJ_CREATE(BigBuilding);
    virtual void update(float delta);
    virtual void onEnter();
    virtual void onExit();
};

#endif /* defined(__CreazyBomber__BigBuilding__) */
