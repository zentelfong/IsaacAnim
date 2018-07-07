#include "AnimationFrame.h"

NS_GM_BEGIN

AnimationFrame::AnimationFrame()
:xPosition(0.0f),
yPosition(0.0f),
xScale(1.0f),
yScale(1.0f),
delay(1),
visible(true),
rotation(0.0f),
interpolated(true)
{

}

/*
<Frame XPosition="0" YPosition="0" XScale="100" YScale="100" Delay="13" Visible="true"
RedTint="255" GreenTint="255" BlueTint="255" AlphaTint="255" RedOffset="0" GreenOffset="0"
BlueOffset="0" Rotation="0" Interpolated="false"/>
*/
//加载动画帧
void AnimationFrame::load(const tinyxml2::XMLElement * elem)
{
	xPosition = elem->FloatAttribute("XPosition");
	yPosition = elem->FloatAttribute("YPosition");
	xScale = elem->FloatAttribute("XScale");
	yScale = elem->FloatAttribute("YScale");
	delay = elem->IntAttribute("Delay");
	visible = elem->BoolAttribute("Visible");
	color.r = (GLubyte)elem->IntAttribute("RedTint");
	color.g = (GLubyte)elem->IntAttribute("GreenTint");
	color.b = (GLubyte)elem->IntAttribute("BlueTint");
	color.a = (GLubyte)elem->IntAttribute("AlphaTint");

	rgbOffset.r = (GLubyte)elem->IntAttribute("RedOffset");
	rgbOffset.g = (GLubyte)elem->IntAttribute("GreenOffset");
	rgbOffset.b = (GLubyte)elem->IntAttribute("BlueOffset");

	rotation = elem->FloatAttribute("Rotation");

	interpolated = elem->BoolAttribute("Interpolated");
}



LayerAnimationFrame::LayerAnimationFrame()
: spriteFrame(nullptr),
xPivot(0),
yPivot(0),
xCrop(0),
yCrop(0),
width(0),
height(0)
{}

LayerAnimationFrame::~LayerAnimationFrame()
{
	CC_SAFE_RELEASE(spriteFrame);
}

void LayerAnimationFrame::setSpriteFrame(SpriteFrame* frame)
{
	CC_SAFE_RELEASE(spriteFrame);
	spriteFrame = frame;
	spriteFrame->retain();
}

//加载时设置spriteFrame
void LayerAnimationFrame::load(const tinyxml2::XMLElement * elem)
{
	AnimationFrame::load(elem);
	xPivot = elem->IntAttribute("XPivot");
	yPivot = elem->IntAttribute("YPivot");
	xCrop = elem->IntAttribute("XCrop");
	yCrop = elem->IntAttribute("YCrop");
	width = elem->IntAttribute("Width");
	height = elem->IntAttribute("Height");

}

NS_GM_END

