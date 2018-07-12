#include "Animation.h"
#include "AnimationSprite.h"
NS_GM_BEGIN
AnimationMgr* AnimationMgr::s_instance=NULL;

bool Animation::load(const tinyxml2::XMLElement* elem)
{
	clear();
	m_name = safeStr(elem->Attribute("Name"));
	m_frameCount = elem->IntAttribute("FrameNum");
	m_loop = elem->BoolAttribute("Loop");

	auto rootAnim = elem->FirstChildElement("RootAnimation");
	if (rootAnim)
	{
		m_rootAnimation = new RootAnimation();
		m_rootAnimation->load(rootAnim);
	}

	auto layerAnims = elem->FirstChildElement("LayerAnimations");
	if (layerAnims)
	{
		auto layerAnim = layerAnims->FirstChildElement("LayerAnimation");
		while (layerAnim)
		{
			LayerAnimation* anim = new LayerAnimation();
			anim->load(layerAnim);
			m_layerAnimations.pushBack(anim);
			anim->release();
			layerAnim = layerAnim->NextSiblingElement();
		}
	}

	auto nullAnims = elem->FirstChildElement("NullAnimations");
	if (nullAnims)
	{
		auto nullAnim = nullAnims->FirstChildElement("NullAnimation");
		while (nullAnim)
		{
			NullAnimation* anim = new NullAnimation();
			anim->load(nullAnim);
			m_nullAnimations.pushBack(anim);
			anim->release();
			nullAnim = nullAnim->NextSiblingElement();
		}

	}

	auto triggers = elem->FirstChildElement("Triggers");
	if (triggers)
	{
		auto trigger = triggers->FirstChildElement("Trigger");
		while (trigger)
		{
			Trigger t;
			t.atFrame = trigger->IntAttribute("AtFrame");
			t.eventId = trigger->IntAttribute("EventId");
			m_triggers.push_back(t);
			trigger = trigger->NextSiblingElement();
		}
	}
	return true;
}

void Animation::clear()
{
	if (m_rootAnimation)
	{
		m_rootAnimation->release();
		m_rootAnimation = NULL;
	}
	m_layerAnimations.clear();
	m_nullAnimations.clear();
	m_triggers.clear();
}

void Animation::update(Node* node, int curFrame)
{
	if (!m_rootAnimation)
		return;

	const AnimationFrame * rootAnim = m_rootAnimation->getFrame();
	if (!rootAnim)
		return;

	//更新layer帧
	for (auto layer : m_layerAnimations)
	{
		float dur=0;
		int idx = layer->getIndexFromTick(curFrame,dur);
		LayerAnimationFrame* frame = layer->getFrame(idx);
		if (!frame)
			continue;
		LayerAnimationFrame* nextFrame = layer->getFrame(idx+1);

		if (!frame->spriteFrame)
		{
			cocos2d::Rect rc;
			rc.origin.x = frame->xCrop;
			rc.origin.y = frame->yCrop;
			rc.size.width = frame->width;
			rc.size.height = frame->height;
			cocos2d::Vec2 pos;
			cocos2d::Size sz=rc.size;
			frame->setSpriteFrame(SpriteFrame::create(m_animations->getSpritePath(layer->getLayerId()), rc, false, pos,sz));
		}

		AnimationLayerItem* sprite = static_cast<AnimationLayerItem*>(node->getChildByTag(layer->getLayerId()));
		if (!sprite)
			continue;

		if (nextFrame && frame->interpolated)
		{
			sprite->setVisible(frame->visible);
			sprite->setPositionX(rootAnim->xPosition + frame->xPosition + (nextFrame->xPosition - frame->xPosition)* dur);
			sprite->setPositionY(-(rootAnim->yPosition + frame->yPosition + (nextFrame->yPosition - frame->yPosition)* dur));
			sprite->setRotation(rootAnim->rotation + frame->rotation + (nextFrame->rotation - frame->rotation)* dur);
			sprite->setAnchorPoint(cocos2d::Vec2((float)frame->xPivot / frame->width, (float)(frame->height - frame->yPivot) / frame->height));
			sprite->setScaleX(rootAnim->xScale * (frame->xScale + (nextFrame->xScale - frame->xScale)* dur) / 10000);
			sprite->setScaleY(rootAnim->yScale * (frame->yScale + (nextFrame->yScale - frame->yScale)* dur) / 10000);
			sprite->setColor(cocos2d::Color3B(
				rootAnim->color.r * (frame->color.r + (nextFrame->color.r - frame->color.r)* dur)/255,
				rootAnim->color.g * (frame->color.g + (nextFrame->color.g - frame->color.g)* dur)/255,
				rootAnim->color.b * (frame->color.b + (nextFrame->color.b - frame->color.b)* dur)/255));

			sprite->setOffsetColor(cocos2d::Color3B(
				rootAnim->rgbOffset.r + frame->rgbOffset.r + (nextFrame->rgbOffset.r - frame->rgbOffset.r)* dur,
				rootAnim->rgbOffset.g + frame->rgbOffset.g + (nextFrame->rgbOffset.g - frame->rgbOffset.g)* dur,
				rootAnim->rgbOffset.b + frame->rgbOffset.b + (nextFrame->rgbOffset.b - frame->rgbOffset.b)* dur));


			sprite->setOpacity(rootAnim->color.a * (frame->color.a + (nextFrame->color.a - frame->color.a)* dur)/255);
			sprite->setSpriteFrame(frame->spriteFrame);
		}
		else
		{
			sprite->setVisible(frame->visible);
			sprite->setPositionX(rootAnim->xPosition + frame->xPosition);
			sprite->setPositionY(-(rootAnim->yPosition + frame->yPosition));
			sprite->setRotation(rootAnim->rotation + frame->rotation);
			sprite->setAnchorPoint(cocos2d::Vec2((float)frame->xPivot / frame->width, (float)(frame->height - frame->yPivot) / frame->height));
			sprite->setScaleX(rootAnim->xScale *frame->xScale / 10000);
			sprite->setScaleY(rootAnim->yScale *frame->yScale / 10000);
			sprite->setColor(cocos2d::Color3B(rootAnim->color.r * frame->color.r / 255, rootAnim->color.g * frame->color.g / 255, rootAnim->color.b * frame->color.b/255));
			sprite->setOffsetColor(cocos2d::Color3B(rootAnim->rgbOffset.r + frame->rgbOffset.r, rootAnim->rgbOffset.g + frame->rgbOffset.g, rootAnim->rgbOffset.b + frame->rgbOffset.b));
			sprite->setOpacity(rootAnim->color.a * frame->color.a/255);
			sprite->setSpriteFrame(frame->spriteFrame);
		}

	}



	//更新NULL帧
	for (auto anims : m_nullAnimations)
	{
		float dur = 0;
		int idx = anims->getIndexFromTick(curFrame, dur);
		AnimationFrame* frame = anims->getFrame(idx);
		if (!frame)
			continue;
		AnimationFrame* nextFrame = anims->getFrame(idx + 1);

		AnimationNullItem* sprite = static_cast<AnimationNullItem*>(node->getChildByTag(anims->getNullId() + 0xffff));
		if (!sprite)
			continue;
		if (nextFrame && frame->interpolated)
		{
			sprite->setVisible(frame->visible);
			sprite->setPositionX(rootAnim->xPosition + frame->xPosition + (nextFrame->xPosition - frame->xPosition)* dur);
			sprite->setPositionY(-(rootAnim->yPosition + frame->yPosition + (nextFrame->yPosition - frame->yPosition)* dur));
			sprite->setRotation(rootAnim->rotation + frame->rotation + (nextFrame->rotation - frame->rotation)* dur);
			sprite->setScaleX(rootAnim->xScale * (frame->xScale + (nextFrame->xScale - frame->xScale)* dur) / 10000);
			sprite->setScaleY(rootAnim->yScale * (frame->yScale + (nextFrame->yScale - frame->yScale)* dur) / 10000);
			sprite->setColor(cocos2d::Color3B(
				rootAnim->color.r * (frame->color.r + (nextFrame->color.r - frame->color.r)* dur) / 255,
				rootAnim->color.g * (frame->color.g + (nextFrame->color.g - frame->color.g)* dur) / 255,
				rootAnim->color.b * (frame->color.b + (nextFrame->color.b - frame->color.b)* dur) / 255));

			sprite->setOffsetColor(cocos2d::Color3B(
				rootAnim->rgbOffset.r + frame->rgbOffset.r + (nextFrame->rgbOffset.r - frame->rgbOffset.r)* dur,
				rootAnim->rgbOffset.g + frame->rgbOffset.g + (nextFrame->rgbOffset.g - frame->rgbOffset.g)* dur,
				rootAnim->rgbOffset.b + frame->rgbOffset.b + (nextFrame->rgbOffset.b - frame->rgbOffset.b)* dur));
			sprite->setOpacity(rootAnim->color.a * (frame->color.a + (nextFrame->color.a - frame->color.a)* dur) / 255);
		}
		else
		{
			sprite->setVisible(frame->visible);
			sprite->setPositionX(rootAnim->xPosition + frame->xPosition);
			sprite->setPositionY(-(rootAnim->yPosition + frame->yPosition));
			sprite->setRotation(rootAnim->rotation + frame->rotation);
			sprite->setScaleX(rootAnim->xScale *frame->xScale / 10000);
			sprite->setScaleY(rootAnim->yScale *frame->yScale / 10000);
			sprite->setColor(cocos2d::Color3B(rootAnim->color.r * frame->color.r / 255, rootAnim->color.g * frame->color.g / 255, rootAnim->color.b * frame->color.b / 255));
			sprite->setOffsetColor(cocos2d::Color3B(rootAnim->rgbOffset.r + frame->rgbOffset.r, rootAnim->rgbOffset.g + frame->rgbOffset.g, rootAnim->rgbOffset.b + frame->rgbOffset.b));
			sprite->setOpacity(rootAnim->color.a * frame->color.a / 255);
		}

	}
}








std::string getFilePath(const std::string& file)
{
	auto find=file.rfind('/');

	if (find != file.npos)
	{
		return file.substr(0,find+1);
	}
	else
		return "";
}

//加载动画文件动画图片可以单独加载
bool Animations::load(const std::string& animFile, bool loadGrap)
{
	bool ret = false;
	m_path = getFilePath(animFile);
	std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(animFile);

	cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(filePath);

	if (!data.isNull())
	{
		tinyxml2::XMLDocument xml;
		do
		{
			CC_BREAK_IF(xml.Parse((const char *)data.getBytes(), data.getSize()) != tinyxml2::XMLError::XML_NO_ERROR);
			auto root = xml.RootElement();

			auto info = root->FirstChildElement("Info");
			if (info)
				m_fps = info->IntAttribute("Fps");

			auto content = root->FirstChildElement("Content");
			if (content)
			{
				auto spriteSheets = content->FirstChildElement("Spritesheets");
				if (spriteSheets)
				{
					m_spritesheets.clear();
					auto sheet = spriteSheets->FirstChildElement("Spritesheet");
					while (sheet)
					{
						SpriteSheetData data;
						data.id = sheet->IntAttribute("Id");
						data.path = m_path + safeStr(sheet->Attribute("Path"));
						m_spritesheets.push_back(data);
						sheet = sheet->NextSiblingElement();
					}
				}

				auto layers = content->FirstChildElement("Layers");
				if (layers)
				{
					m_layers.clear();
					auto layer = layers->FirstChildElement("Layer");
					while (layer)
					{
						LayerData data;
						data.id = layer->IntAttribute("Id");
						data.name = safeStr(layer->Attribute("Name"));
						data.spritesheetid = layer->IntAttribute("SpritesheetId");
						m_layers.push_back(data);
						layer = layer->NextSiblingElement();
					}
				}

				//nulls的解析
				auto nulls = content->FirstChildElement("Nulls");
				if (nulls)
				{
					m_nulls.clear();
					auto null = nulls->FirstChildElement("Null");
					while (null)
					{
						NullData data;
						data.id = null->IntAttribute("Id");
						data.name = safeStr(null->Attribute("Name"));
						m_nulls.push_back(data);
						null = null->NextSiblingElement();
					}
				}

				//事件id的解析
				auto events = content->FirstChildElement("Events");
				if (events)
				{
					m_events.clear();
					auto event = events->FirstChildElement("Event");
					while (event)
					{
						EventData data;
						data.id = event->IntAttribute("Id");
						data.name = safeStr(event->Attribute("Name"));
						m_events.push_back(data);
						event = event->NextSiblingElement();
					}
				}
			}

			auto animations = root->FirstChildElement("Animations");
			if (animations)
			{
				m_defaultAnimation = safeStr(animations->Attribute("DefaultAnimation"));
				auto animation = animations->FirstChildElement("Animation");
				while (animation)
				{
					Animation* anim = new Animation(this);
					if (anim->load(animation))
					{
						m_animationMap.insert(anim->getName(), anim);
					}
					anim->release();
					animation = animation->NextSiblingElement();
				}
			}
			if (loadGrap)
				loadGraphics();
			return true;

		} while (0);
		return false;
	}
	else
		return false;
}


NS_GM_END
