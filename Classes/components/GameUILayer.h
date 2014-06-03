//
//  GameUILayer.h
//  CreazyBomber
//
//  Created by AIRIA on 14-6-3.
//
//

#ifndef __CreazyBomber__GameUILayer__
#define __CreazyBomber__GameUILayer__

#include "base/BaseLayer.h"

class GameUILayer:public BaseLayer
{
public:
    virtual void onEnter();
    virtual void onExit();
    virtual void onTexturesLoaded();
    virtual bool init();
    CREATE_FUNC(GameUILayer);
};

#endif /* defined(__CreazyBomber__GameUILayer__) */