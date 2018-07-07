#pragma once
#include "../common.h"

NS_GM_BEGIN

//����֡����
class CC_DLL AnimationFrame :public Ref
{
public:
	AnimationFrame();

	/*
	<Frame XPosition="0" YPosition="0" XScale="100" YScale="100" Delay="13" Visible="true" 
	RedTint="255" GreenTint="255" BlueTint="255" AlphaTint="255" RedOffset="0" GreenOffset="0" 
	BlueOffset="0" Rotation="0" Interpolated="false"/>
	*/
	//���ض���֡
	void load(const tinyxml2::XMLElement * elem);

	float xPosition;//λ��
	float yPosition;
	float xScale;//����
	float yScale;
	int delay;//֡��
	bool visible;
	Color4B color;
	Color3B rgbOffset;
	float rotation;
	bool interpolated;
};

//�㶯��
class CC_DLL LayerAnimationFrame :public AnimationFrame
{
public:
	LayerAnimationFrame();
	~LayerAnimationFrame();

	void setSpriteFrame(SpriteFrame* frame);

	//����ʱ����spriteFrame
	void load(const tinyxml2::XMLElement * elem);

public:
	SpriteFrame* spriteFrame;//�����֡
	int xPivot;//��ת�����ĵ�
	int yPivot;
	int xCrop;//��֡ͼƬ��λ��
	int yCrop;
	int width;//��֡ͼƬ�ĳߴ�
	int height;
};

NS_GM_END

