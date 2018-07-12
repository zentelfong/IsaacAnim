#pragma once
#include "AnimationFrame.h"
NS_GM_BEGIN

class Animations;
class Animation;

class CC_DLL LayerAnimation :public Ref
{
public:
	LayerAnimation()
		:m_layerId(-1), m_visible(false)
	{}

	~LayerAnimation()
	{
		clear();
	}

	inline int getFrameCount() const
	{
		return m_frames.size();
	}

	inline LayerAnimationFrame* getFrame(int idx) const
	{
		if (m_frames.empty())
			return NULL;
		if (idx >= m_frames.size())
			return NULL;
		return m_frames.at(idx);
	}

	inline int getIndexFromTick(int tick,float &dur)
	{
		int frameIdx = 0;
		for (size_t i = 0; i<m_frames.size();++i)
		{
			if (frameIdx + m_frames.at(i)->delay > tick)
			{
				dur = (float)(tick - frameIdx) / (float)m_frames.at(i)->delay;
				return i;
			}
			else
				frameIdx += m_frames.at(i)->delay;
		}
		return 0;
	}

	inline void clear()
	{
		m_frames.clear();
	}

	inline bool isVisible()
	{
		return m_visible;
	}

	inline int getLayerId()
	{
		return m_layerId;
	}

	/*
	<LayerAnimation LayerId="1" Visible="true">
		<Frame XPosition="0" YPosition="2" XPivot="16" YPivot="28" XCrop="0" YCrop="0" Width="32" Height="32" XScale="80" 
		YScale="120" Delay="1" Visible="true" RedTint="203" GreenTint="60" BlueTint="60" AlphaTint="255" RedOffset="61" 
		GreenOffset="0" BlueOffset="0" Rotation="0" Interpolated="false"/>
	</LayerAnimation>
	*/
	void load(const tinyxml2::XMLElement * elem)
	{
		clear();
		m_layerId = elem->IntAttribute("LayerId");
		m_visible = elem->BoolAttribute("Visible");

		auto frameElem= elem->FirstChildElement("Frame");
		while (frameElem)
		{
			LayerAnimationFrame* frame = new LayerAnimationFrame();
			frame->load(frameElem);
			frameElem = frameElem->NextSiblingElement();
			m_frames.pushBack(frame);
			frame->release();
		}
	}

private:
	int		   m_layerId;
	bool       m_visible;
	Vector<LayerAnimationFrame*> m_frames;
};

class CC_DLL NullAnimation :public Ref
{
public:
	NullAnimation()
		:m_nullId(-1), m_visible(false)
	{}
	~NullAnimation()
	{
	
	}

	inline int getFrameCount() const
	{
		return m_frames.size();
	}

	inline AnimationFrame* getFrame(int idx) const
	{
		if (m_frames.empty())
			return NULL;
		if (idx >= m_frames.size())
			return NULL;
		return m_frames.at(idx);
	}

	inline int getIndexFromTick(int tick, float &dur)
	{
		int frameIdx = 0;
		for (size_t i = 0; i<m_frames.size(); ++i)
		{
			if (frameIdx + m_frames.at(i)->delay > tick)
			{
				dur = (float)(tick - frameIdx) / (float)m_frames.at(i)->delay;
				return i;
			}
			else
				frameIdx += m_frames.at(i)->delay;
		}
		return 0;
	}

	inline void clear()
	{
		m_frames.clear();
	}

	bool isVisible()
	{
		return m_visible;
	}

	int getNullId()
	{
		return m_nullId;
	}

	/*
	<NullAnimation NullId="5" Visible="true">
	<Frame XPosition="0" YPosition="-12" XScale="197" YScale="254" Delay="1" Visible="true" 
	RedTint="255" GreenTint="255" BlueTint="255" AlphaTint="255" RedOffset="0" GreenOffset="0" 
	BlueOffset="0" Rotation="0" Interpolated="false"/>
	</NullAnimation>
	*/

	void load(const tinyxml2::XMLElement * elem)
	{
		clear();
		m_nullId = elem->IntAttribute("NullId");
		m_visible = elem->BoolAttribute("Visible");

		auto frameElem = elem->FirstChildElement("Frame");
		while (frameElem)
		{
			AnimationFrame* frame = new AnimationFrame();
			frame->load(frameElem);
			frameElem = frameElem->NextSiblingElement();
			m_frames.pushBack(frame);
			frame->release();
		}
	}
private:
	int m_nullId;
	bool m_visible;
	Vector<AnimationFrame*> m_frames;
};


class CC_DLL RootAnimation :public Ref
{
public:
	RootAnimation()
	{}

	const AnimationFrame* getFrame()
	{
		return &m_frame;
	}

	/*
	<RootAnimation>
	<Frame XPosition="0" YPosition="0" XScale="100" YScale="100" Delay="4" Visible="true" 
	RedTint="255" GreenTint="255" BlueTint="255" AlphaTint="255" RedOffset="0" GreenOffset="0"
	BlueOffset="0" Rotation="0" Interpolated="false"/>
	</RootAnimation>
	*/

	void load(const tinyxml2::XMLElement * elem)
	{
		auto frameElem = elem->FirstChildElement("Frame");
		if (frameElem)
		{
			m_frame.load(frameElem);
		}
	}
private:
	AnimationFrame m_frame;
};





//����
class CC_DLL Animation :public Ref
{
public:
	struct Trigger
	{
		int eventId;
		int atFrame;
	};

	Animation(Animations* animations)
		:m_animations(animations), m_rootAnimation(NULL)
	{
	}
	~Animation()
	{
		clear();
	}

	inline Animations* getAnimations()
	{
		return m_animations;
	}

	/*
	<Animation Name="Hit" FrameNum="8" Loop="false">
		<RootAnimation>
		</RootAnimation>
		<LayerAnimations>
		</LayerAnimations>
		<NullAnimations>
		</NullAnimations>
	</Animation>
	*/
	bool load(const tinyxml2::XMLElement* elem);

	void clear();

	//���¶���֡
	void update(Node* node, int curFrame);

	inline int getFrameCount(){return m_frameCount;}
	inline bool isLoop(){ return m_loop; }
	inline const std::string & getName(){ return m_name; }

	RootAnimation* getRootAnimation(){ return m_rootAnimation; }
	const Vector<LayerAnimation*>& getLayerAnimations(){return m_layerAnimations;}
	const Vector<NullAnimation*>&  getNullAnimations(){ return m_nullAnimations; }
	
	const std::vector<Trigger>& getTriggers(){ return m_triggers; }

protected:
	Animations*             m_animations;
	RootAnimation*			m_rootAnimation;
	Vector<LayerAnimation*> m_layerAnimations;//��Ӧָ����sprite�����ж���
	Vector<NullAnimation*>	m_nullAnimations;


	std::vector<Trigger>	m_triggers;//�� frame,ֵ eventid
	std::string				m_name;//��������
	int						m_frameCount;//��֡��
	bool					m_loop;	   //�Ƿ�ѭ�����Ŷ���
};


//������
class CC_DLL Animations :public Ref
{
public:
	Animations()
		:m_fps(60)
	{
	
	}
	~Animations()
	{
		for (auto i : m_spritesheets)
		{
			cocos2d::TextureCache::getInstance()->removeTextureForKey(i.path);
		}
	}

	//���ض����ļ�����ͼƬ���Ե�������
	bool load(const std::string& animFile, bool loadGraphics);

	inline void loadGraphics()
	{
		for (auto i : m_spritesheets)
		{
			//ͼƬ���ص���ͼ������
			cocos2d::TextureCache::getInstance()->addImage(i.path);
		}
	}

	inline const std::string& getDefaultAnimationName()
	{
		return m_defaultAnimation;
	}

	inline Animation* getAnimation(const std::string& animName)
	{
		auto find = m_animationMap.find(animName);
		if (find != m_animationMap.end())
			return find->second;
		else
			return NULL;
	}

	const std::string getSpritePath(int layerid)
	{
		for (const auto& layer : m_layers)
		{
			if (layer.id == layerid)
			{
				for (const auto& spritesheet : m_spritesheets)
				{
					if (layer.spritesheetid==spritesheet.id)
					{
						return spritesheet.path;
					}
				}
			}
		}
		return "";
	}

	int getNullIdByName(const std::string& name)
	{
		for (const auto& layer : m_nulls)
		{
			if (layer.name == name)
			{
				return layer.id;
			}
		}
		return -1;
	}

	int getLayerIdByName(const std::string& name)
	{
		for (const auto& layer : m_layers)
		{
			if (layer.name == name)
			{
				return layer.id;
			}
		}
		return -1;
	}

	const std::string getEventName(int id)
	{
		for (const auto& ev : m_events)
		{
			if (ev.id == id)
			{
				return ev.name;
			}
		}
		return "";
	}

	inline int getFps()
	{
		return m_fps;
	}

	inline float getFpsInterval()
	{
		return 1.0/(float)m_fps;
	}
private:
	struct SpriteSheetData
	{
		int id;
		std::string path;
	};
	struct LayerData
	{
		int id;
		int spritesheetid;
		std::string name;
	};

	struct NullData
	{
		int id;
		std::string name;
	};

	struct EventData
	{
		int id;
		std::string name;
	};

	std::string m_path;//��ǰ�����ļ�����Ŀ¼�������õ�ͼƬ��Դ������ڸ�Ŀ¼
	std::vector<SpriteSheetData> m_spritesheets;//�������õ�ͼƬ�ļ�
	std::vector<LayerData> m_layers;
	std::vector<NullData> m_nulls;
	std::vector<EventData> m_events;
	int m_fps;
	Map<std::string, Animation*> m_animationMap;
	std::string	m_defaultAnimation;
};


//����������������Animation�����ڶ����ļ�Ԥ���ص�
class CC_DLL AnimationMgr
{
public:
	AnimationMgr()
	{

	}

	~AnimationMgr()
	{

	}

	static AnimationMgr* instance()
	{
		if (!s_instance)
			s_instance = new AnimationMgr();
		return s_instance;
	}


	Animations* getAnimations(const std::string& animFile)
	{
		auto anim = m_animationCache.find(animFile);
		if (anim == m_animationCache.end())
		{
			return load(animFile, true);
		}
		else
			return anim->second;
	}

	Animations* load(const std::string& animFile, bool loadGraphics)
	{
		Animations* map = new Animations();
		if (!map->load(animFile, loadGraphics))
		{
			delete map;
			return NULL;
		}
		m_animationCache.insert(animFile, map);
		map->release();
		return map;
	}
private:
	static AnimationMgr* s_instance;
	Map<std::string, Animations* > m_animationCache;
};


NS_GM_END

