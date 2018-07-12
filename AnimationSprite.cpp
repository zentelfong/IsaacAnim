#include "AnimationSprite.h"


NS_GM_BEGIN

#define STRINGIFY(A)  #A

const char* ccPositionTextureOffsetColor_noMVP_frag = STRINGIFY(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec3 u_colorOffset;

void main()
{
	//gl_FragColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	vec4 color = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	gl_FragColor = vec4(color.rgb + u_colorOffset.rgb * color.a, color.a);
}
);

static std::string offsetColorFrag="gameOffsetColorFrag";


AnimationLayerItem::AnimationLayerItem()
:m_offsetColor(cocos2d::Color3B::BLACK)
{
}

AnimationLayerItem::~AnimationLayerItem()
{

}

AnimationLayerItem* AnimationLayerItem::create()
{
	AnimationLayerItem *sprite = new (std::nothrow) AnimationLayerItem();
	if (sprite && sprite->init())
	{
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}


cocos2d::GLProgramState* getProgramState(cocos2d::Color3B offsetColor)
{
	auto glprogram = cocos2d::GLProgramCache::getInstance()->getGLProgram(offsetColorFrag);
	if (!glprogram)
	{
		glprogram = cocos2d::GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_noMVP_vert, ccPositionTextureOffsetColor_noMVP_frag);
		cocos2d::GLProgramCache::getInstance()->addGLProgram(glprogram, offsetColorFrag);
	}

	auto glprogramState = cocos2d::GLProgramState::create(glprogram);

	glprogramState->setUniformVec3("u_colorOffset", cocos2d::Vec3(offsetColor.r / 255.0f, offsetColor.g / 255.0f, offsetColor.b / 255.0f));

	return glprogramState;
}

bool AnimationLayerItem::initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect& rect, bool rotated)
{
	if (Sprite::initWithTexture(texture, rect, rotated))
	{
		setGLProgramState(getProgramState(m_offsetColor));
		return true;
	}
	else
		return false;
}

void AnimationLayerItem::setOffsetColor(cocos2d::Color3B offsetColor)
{
	if (m_offsetColor == offsetColor)
		return;
	m_offsetColor = offsetColor;
	if (getGLProgramState())
		getGLProgramState()->setUniformVec3("u_colorOffset", cocos2d::Vec3(offsetColor.r / 255.0f, offsetColor.g / 255.0f, offsetColor.b / 255.0f));
}


cocos2d::Color3B AnimationLayerItem::getOffsetColor()
{
	return m_offsetColor;
}


AnimationNullItem* AnimationNullItem::create() 
{
	AnimationNullItem *item = new (std::nothrow) AnimationNullItem();
	if (item && item->init())
	{
		item->autorelease();
		return item;
	}
	CC_SAFE_DELETE(item);
	return nullptr;
}

AnimationSprite::AnimationSprite()
:m_animations(nullptr), m_animation(nullptr), m_curFrame(0), m_state(State::Default), m_lastUpdate(0)
{

}

AnimationSprite::~AnimationSprite()
{
	if (m_animations)
		m_animations->release();
	if (m_animation)
		m_animation->release();
}

AnimationSprite* AnimationSprite::create()
{
	AnimationSprite *sprite = new (std::nothrow) AnimationSprite();
	if (sprite)
	{
		sprite->autorelease();
		return sprite;
	}
	return nullptr;
}

AnimationSprite* AnimationSprite::create(const std::string& animationFile)
{
	AnimationSprite *sprite = new (std::nothrow) AnimationSprite();
	if (sprite && sprite->setFileName(animationFile))
	{
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}


bool AnimationSprite::setFileName(const std::string& animationFile)
{
	m_animationFile = animationFile;
	CC_SAFE_RELEASE_NULL(m_animations);
	m_animations = AnimationMgr::instance()->getAnimations(m_animationFile);
	if (m_animations)
	{
		m_animations->retain();
		return true;
	}
	else
		return false;	
}

Animations* AnimationSprite::getAnimations()
{
	return m_animations;
}

const std::string& 	AnimationSprite::getFilename()
{
	return m_animationFile;
}

bool AnimationSprite::setAnimation(const std::string& animationName)
{
	if (!m_animations)
		return false;
	auto animation = m_animations->getAnimation(animationName);
	if (!animation)
		return false;
	setAnimation(animation);
	return true;
}

bool AnimationSprite::isFinished(const std::string& animationName)
{
	return m_state != State::Playering;
}

bool AnimationSprite::play(const std::string& animationName, bool force)
{
	if (!force && m_state == State::Playering)
		return true;

	if (setAnimation(animationName))
	{
		m_state = State::Playering;
		scheduleUpdate();
		return true;
	}
	else
	{
		unscheduleUpdate();
		return false;
	}
}

void AnimationSprite::update(float delta)
{
	if (!m_animation)
		return;

	m_lastUpdate += delta;
	if (m_lastUpdate >= m_animations->getFpsInterval())
	{
		m_lastUpdate = 0;
		m_animation->update(this, m_curFrame);

		auto triggers=m_animation->getTriggers();
		for (const auto& trigger : triggers)
		{
			if (trigger.atFrame == m_curFrame)
			{
				auto name=m_animations->getEventName(trigger.eventId);
				onEventTriggered(name);
			}
		}

		if (m_state == State::Playering)
		{
			++m_curFrame;
			if (m_curFrame >= m_animation->getFrameCount())
			{
				if (!m_animation->isLoop())
					stop();
				else
					m_curFrame = 0;
			}	
		}
	}	
}

bool AnimationSprite::reset()
{
	m_curFrame = 0;
	return true;
}


void AnimationSprite::stop()
{
	m_curFrame = 0;
	m_state = State::Stoped;
	unscheduleUpdate();
}

bool AnimationSprite::setFrame(int frameNum)
{
	if (frameNum >= 0 && m_animation && frameNum < m_animation->getFrameCount())
	{
		m_curFrame = frameNum;
		return true;
	}
	else
		return false;
}

int AnimationSprite::getFrame()
{
	return m_curFrame;
}

int AnimationSprite::getFrameCount()
{
	if (m_animation)
		return m_animation->getFrameCount();
	else
		return 0;
}

void AnimationSprite::setLastFrame()
{
	m_curFrame = getFrameCount();
}

bool AnimationSprite::isPlaying(const std::string& AnimationName)
{
	return m_state == State::Playering;
}


void AnimationSprite::onEventTriggered(const std::string& EventName)
{
	if (m_eventHandler)
		m_eventHandler(this,EventName);
}

int AnimationSprite::getLayerCount()
{
	if (m_animation)
		return m_animation->getLayerAnimations().size();
	else
		return -1;
}

const std::string AnimationSprite::getDefaultAnimationName()
{
	if (m_animations)
		return m_animations->getDefaultAnimationName();
	else
		return "";
}

void AnimationSprite::setEventHandler(std::function<void(AnimationSprite*,const std::string&)> func)
{
	m_eventHandler = func;
}

void AnimationSprite::removeEventHandler()
{
	m_eventHandler = NULL;
}

void AnimationSprite::setAnimation(Animation* animation)
{
	CC_SAFE_RELEASE_NULL(m_animation);
	if (!animation)
		return;
	
	m_animation = animation;
	m_animation->retain();
	m_curFrame = 0;
	//ÉèÖÃ×Ósprite
	auto layerAnimations = m_animation->getLayerAnimations();
	for (auto animation :layerAnimations)
	{
		cocos2d::Node* node = getChildByTag(animation->getLayerId());
		if (!node)
		{
			node = AnimationLayerItem::create();
			node->setTag(animation->getLayerId());
			addChild(node);
		}
		node->setLocalZOrder(animation->getLayerId());
		node->setVisible(animation->isVisible());
	}

	auto nullAnimations = m_animation->getNullAnimations();
	for (auto animation : nullAnimations)
	{
		AnimationNullItem* node =static_cast<AnimationNullItem*>(getChildByTag(animation->getNullId() + 0xffff));
		if (!node)
		{
			node = AnimationNullItem::create();
			node->setTag(animation->getNullId() + 0xffff);
			addChild(node);
		}
		node->setLocalZOrder(animation->getNullId());
		node->setVisible(animation->isVisible());
	}


}



AnimationNullItem* AnimationSprite::findNullItem(const std::string& name)
{
	if (!m_animations)
		return NULL;
	int id = m_animations->getNullIdByName(name);
	if (id < 0)
		return NULL;
	return static_cast<AnimationNullItem*>(getChildByTag(id + 0xffff));
}


AnimationLayerItem* AnimationSprite::findLayerItem(const std::string& name)
{
	if (!m_animations)
		return NULL;
	int id = m_animations->getLayerIdByName(name);
	if (id < 0)
		return NULL;
	return static_cast<AnimationLayerItem*>(getChildByTag(id));
}



























NS_GM_END

