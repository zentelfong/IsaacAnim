#pragma once
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#define NS_GM_BEGIN                     namespace game {
#define NS_GM_END                       }
#define USING_NS_GM                     using namespace game
#define NS_GM                           ::game


NS_GM_BEGIN

inline const char* safeStr(const char* str)
{
	if (str)
		return str;
	else
		return "";
}


using cocos2d::Ref;
using cocos2d::Color4B;
using cocos2d::Color3B;
using cocos2d::Vector;
using cocos2d::Map;

using cocos2d::Node;
using cocos2d::Sprite;
using cocos2d::SpriteFrame;


NS_GM_END

