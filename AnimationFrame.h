#pragma once
#include "../common.h"

NS_GM_BEGIN

//动画帧数据
class CC_DLL AnimationFrame :public Ref
{
public:
	AnimationFrame();

	/*
	<Frame XPosition="0" YPosition="0" XScale="100" YScale="100" Delay="13" Visible="true" 
	RedTint="255" GreenTint="255" BlueTint="255" AlphaTint="255" RedOffset="0" GreenOffset="0" 
	BlueOffset="0" Rotation="0" Interpolated="false"/>
	*/
	//加载动画帧
	void load(const tinyxml2::XMLElement * elem);

	float xPosition;//位置
	float yPosition;
	float xScale;//缩放
	float yScale;
	int delay;//帧数
	bool visible;
	Color4B color;
	Color3B rgbOffset;
	float rotation;
	bool interpolated;
};

//层动画
class CC_DLL LayerAnimationFrame :public AnimationFrame
{
public:
	LayerAnimationFrame();
	~LayerAnimationFrame();

	void setSpriteFrame(SpriteFrame* frame);

	//加载时设置spriteFrame
	void load(const tinyxml2::XMLElement * elem);

public:
	SpriteFrame* spriteFrame;//保存该帧
	int xPivot;//旋转的中心点
	int yPivot;
	int xCrop;//该帧图片的位置
	int yCrop;
	int width;//该帧图片的尺寸
	int height;
};

NS_GM_END

