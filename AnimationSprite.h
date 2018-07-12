#pragma once
#include "Animation.h"

NS_GM_BEGIN


//�������
class CC_DLL AnimationLayerItem :public cocos2d::Sprite
{
public:
	AnimationLayerItem();
	~AnimationLayerItem();

	virtual std::string getDescription() const{ return "AnimationLayerItem"; }
	static AnimationLayerItem* create();
	bool initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect& rect, bool rotated);
	void setOffsetColor(cocos2d::Color3B offsetColor);
	cocos2d::Color3B getOffsetColor();
protected:
	cocos2d::Color3B m_offsetColor;
};

//�ն�����������ڽ�ɫ����ֳ�������
class CC_DLL AnimationNullItem :public cocos2d::Node
{
public:
	AnimationNullItem() :m_offsetColor(cocos2d::Color3B::BLACK){}
	~AnimationNullItem(){}

	static AnimationNullItem* create();

	virtual std::string getDescription() const{ return "AnimationNullItem"; }
	void setOffsetColor(cocos2d::Color3B offsetColor){ m_offsetColor = offsetColor; }
	cocos2d::Color3B getOffsetColor(){ return m_offsetColor; }
private:
	cocos2d::Color3B m_offsetColor;
};


//���ж����ľ���
class CC_DLL AnimationSprite :public Node
{
public:
	enum class State{
		Default,
		Playering,
		Stoped
	};

	AnimationSprite();
	~AnimationSprite();

	static AnimationSprite* create();
	static AnimationSprite* create(const std::string& animationFile);

	bool setFileName(const std::string& animationFile);

	Animations* getAnimations();

	bool setAnimation(const std::string& animationName);

	bool isFinished(const std::string& animationName);

	bool play(const std::string& animationName, bool Force);
	
	void stop();

	bool setFrame(int frameNum);

	bool reset();

	const std::string& getFilename();

	int getFrame();

	int getFrameCount();

	void setLastFrame();

	bool isPlaying(const std::string& AnimationName);

	//�¼�������
	void onEventTriggered(const std::string& EventName);

	int getLayerCount();

	const std::string getDefaultAnimationName();

	virtual void update(float delta);

	//����nullitem
	AnimationNullItem* findNullItem(const std::string& name);

	//����layeritem
	AnimationLayerItem* findLayerItem(const std::string& name);

	//�¼��������粥�Ž�ɫ����������ִ����������
	void setEventHandler(std::function<void(AnimationSprite*,const std::string&)> func);
	void removeEventHandler();
private:
	void setAnimation(Animation* animation);
	Animations* m_animations;
	Animation* m_animation;
	std::string m_animationFile;
	std::function<void(AnimationSprite*,const std::string&)> m_eventHandler;
	float m_lastUpdate;
	int m_curFrame;
	State m_state;
};


NS_GM_END
