#include <memory>

#include "Sim/GASSScene.h"
#include "Modules/Graphics/OSGSimpleGraphicsSceneManager.h"
#include "Modules/Graphics/OSGGraphicsSystem.h"
#include "Modules/Graphics/OSGNodeMasks.h"
#include "Modules/Graphics/OSGViewport.h"
#include "Modules/Graphics/OSGDebugDraw.h"
#include "Modules/Graphics/Utils/TextBox.h"
#include "Modules/Graphics/Utils/ViewDependentShadowMapExt.h"
#include "Modules/Graphics/OSGMaterial.h"

namespace GASS
{
	OSGSimpleGraphicsSceneManager::OSGSimpleGraphicsSceneManager(SceneWeakPtr scene) : Reflection(scene),
		m_Fog(new osg::Fog()),
		m_FogStart (200),
		m_FogEnd (400),
		m_UseFog(1),
		m_FogMode (FM_LINEAR),
		m_FogDensity(0.01f),
		m_FogColor(1,1,1),
		m_AmbientColor(0.0,0.0,0.0),
		m_ShadowMaxFarDistance(400),
		m_ShadowTextureSize(2048),
		m_ShadowMinimumNearFarRatio(0.1f),
		m_EnableShadows(true)
	{
	}

	void OSGSimpleGraphicsSceneManager::OnPostConstruction()
	{
		RegisterForPostUpdate<OSGGraphicsSystem>();

		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		ScenePtr scene = GetScene();
		if (!scene)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Scene not present", "OSGSimpleGraphicsSceneManager::OnInitialize");
		}

		m_RootNode = new osg::PositionAttitudeTransform();
		m_RootNode->setName("GASSRootNode");
		m_ShadowRootNode = new osg::Group();
		SetEnableShadows(m_EnableShadows);

		m_LightModel = new osg::LightModel;
		m_LightModel->setTwoSided(false);
		SetAmbientColor(m_AmbientColor);
		m_RootNode->getOrCreateStateSet()->setAttributeAndModes(m_LightModel, osg::StateAttribute::ON);

		m_RootNode->setStateSet(new SimpleMaterial());

		OSGGraphicsSystemPtr gfx_sys = OSGGraphicsSystemPtr(m_GFXSystem);

		osg::StateSet* stateset = m_RootNode->getOrCreateStateSet();
		stateset->setAttributeAndModes(m_Fog.get());

		UpdateFogSettings();

		//add debug node
		m_DebugDraw = new OSGDebugDraw();
		m_RootNode->addChild(m_DebugDraw->GetNode());

		m_RootNode->addChild(gfx_sys->GetDebugText()->getGroup());
		
		Material::SetLighting(m_RootNode->getOrCreateStateSet(), osg::StateAttribute::ON);
		Material::SetReceiveShadows(m_RootNode->getOrCreateStateSet(), osg::StateAttribute::ON);
	}

	OSGSimpleGraphicsSceneManager::~OSGSimpleGraphicsSceneManager(void)
	{
		if(m_ShadowedScene.valid())
			m_ShadowedScene->setShadowTechnique(nullptr);
	}

	void OSGSimpleGraphicsSceneManager::RegisterReflection()
	{
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("OSG Scene Manager", OF_VISIBLE));
		RegisterGetSet("FogMode", &OSGSimpleGraphicsSceneManager::GetFogMode, &OSGSimpleGraphicsSceneManager::SetFogMode, PF_VISIBLE | PF_EDITABLE, "Fog type");
		RegisterGetSet("FogEnd", &OSGSimpleGraphicsSceneManager::GetFogEnd, &OSGSimpleGraphicsSceneManager::SetFogEnd,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FogStart", &OSGSimpleGraphicsSceneManager::GetFogStart, &OSGSimpleGraphicsSceneManager::SetFogStart,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FogDensity", &OSGSimpleGraphicsSceneManager::GetFogDensity, &OSGSimpleGraphicsSceneManager::SetFogDensity,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FogColor", &OSGSimpleGraphicsSceneManager::GetFogColor, &OSGSimpleGraphicsSceneManager::SetFogColor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("AmbientColor", &OSGSimpleGraphicsSceneManager::GetAmbientColor, &OSGSimpleGraphicsSceneManager::SetAmbientColor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ShadowMaxFarDistance", &OSGSimpleGraphicsSceneManager::GetShadowMaxFarDistance, &OSGSimpleGraphicsSceneManager::SetShadowMaxFarDistance, PF_VISIBLE | PF_EDITABLE, "");
		RegisterGetSet("ShadowFadeDistanceRatio", &OSGSimpleGraphicsSceneManager::GetShadowFadeDistanceRatio, &OSGSimpleGraphicsSceneManager::SetShadowFadeDistanceRatio, PF_VISIBLE | PF_EDITABLE, "");
		RegisterGetSet("ShadowMinimumNearFarRatio", &OSGSimpleGraphicsSceneManager::GetShadowMinimumNearFarRatio, &OSGSimpleGraphicsSceneManager::SetShadowMinimumNearFarRatio, PF_VISIBLE | PF_EDITABLE, "");
		RegisterMember("ShadowTextureSize", &OSGSimpleGraphicsSceneManager::m_ShadowTextureSize, PF_VISIBLE, "");
		RegisterGetSet("ShadowSoftness", &OSGSimpleGraphicsSceneManager::GetShadowSoftness, &OSGSimpleGraphicsSceneManager::SetShadowSoftness, PF_VISIBLE, "");
		RegisterGetSet("ShadowEnabled", &OSGSimpleGraphicsSceneManager::GetEnableShadows, &OSGSimpleGraphicsSceneManager::SetEnableShadows, PF_VISIBLE | PF_EDITABLE, "");
	}

	float OSGSimpleGraphicsSceneManager::GetShadowMaxFarDistance() const
	{
		return m_ShadowMaxFarDistance;
	}
	
	void OSGSimpleGraphicsSceneManager::SetShadowMaxFarDistance(float value)
	{
		m_ShadowMaxFarDistance = value;
		if (m_ShadowedScene)
		{
			m_ShadowedScene->getShadowSettings()->setMaximumShadowMapDistance(value);
			if(m_ShadowMaxDistanceUniform)
				m_ShadowMaxDistanceUniform->set(osg::Vec2f(m_ShadowMaxFarDistance, m_ShadowMaxFarDistance * m_ShadowFadeDistanceRatio));
		}
	}

	float OSGSimpleGraphicsSceneManager::GetShadowSoftness() const
	{
		return m_ShadowSoftness;
	}

	void OSGSimpleGraphicsSceneManager::SetShadowSoftness(float value)
	{
		m_ShadowSoftness = value;
		if (m_ShadowSoftnessUniform)
			m_ShadowSoftnessUniform->set(m_ShadowSoftness);
	}

	float OSGSimpleGraphicsSceneManager::GetShadowFadeDistanceRatio() const
	{
		return m_ShadowFadeDistanceRatio;
	}

	void OSGSimpleGraphicsSceneManager::SetShadowFadeDistanceRatio(float value)
	{
		m_ShadowFadeDistanceRatio = value;
		if (m_ShadowedScene)
		{
			if (m_ShadowMaxDistanceUniform)
				m_ShadowMaxDistanceUniform->set(osg::Vec2f(m_ShadowMaxFarDistance, m_ShadowMaxFarDistance * m_ShadowFadeDistanceRatio));
		}
	}

	float OSGSimpleGraphicsSceneManager::GetShadowMinimumNearFarRatio() const
	{
		return m_ShadowMinimumNearFarRatio;
	}

	bool OSGSimpleGraphicsSceneManager::GetEnableShadows() const
	{
		return m_EnableShadows;
	}

	void OSGSimpleGraphicsSceneManager::SetEnableShadows(bool value)
	{
		m_EnableShadows = value;
		if (m_ShadowRootNode) //initialized?
		{
			if (m_EnableShadows)
			{
				if (!m_ShadowedScene)
				{
					m_ShadowedScene = CreateShadowNode();
					m_RootNode->addChild(m_ShadowedScene);
				}

				if (!m_ShadowedScene->containsNode(m_ShadowRootNode))
				{
					m_ShadowedScene->addChild(m_ShadowRootNode);
					if(m_RootNode->containsNode(m_ShadowRootNode))
						m_RootNode->removeChild(m_ShadowRootNode);
				}

				if (m_ShadowedScene)
					m_ShadowedScene->setNodeMask(~0U);
			}
			else
			{
				if (!m_RootNode->containsNode(m_ShadowRootNode))
					m_RootNode->addChild(m_ShadowRootNode);
				if(m_ShadowedScene && m_ShadowedScene->containsNode(m_ShadowRootNode))
					m_ShadowedScene->removeChild(m_ShadowRootNode);

				if (m_ShadowedScene)
					m_ShadowedScene->setNodeMask(0U);
			}
		}
	}

	void OSGSimpleGraphicsSceneManager::SetShadowMinimumNearFarRatio(float value)
	{
		m_ShadowMinimumNearFarRatio = value;
		if (m_ShadowedScene)
			m_ShadowedScene->getShadowSettings()->setMinimumShadowMapNearFarRatio(value);
	}

	void OSGSimpleGraphicsSceneManager::SetAmbientColor(const ColorRGB& value)
	{ 
		m_AmbientColor = value;
		if (m_LightModel)
			m_LightModel->setAmbientIntensity(osg::Vec4(static_cast<float>(m_AmbientColor.r), 
				static_cast<float>(m_AmbientColor.g), 
				static_cast<float>(m_AmbientColor.b)
				,1.0f));
	}

	void OSGSimpleGraphicsSceneManager::OnSceneCreated()
	{
		void* root = static_cast<void*>(m_RootNode.get());
		void* shadow_node = static_cast<void*>(m_ShadowRootNode.get());

		SystemMessagePtr loaded_msg(new GraphicsSceneManagerLoadedEvent(std::string("OSG"),root,shadow_node));
		SimSystemManagerPtr sim_sm = OSGGraphicsSystemPtr(m_GFXSystem)->GetSimSystemManager();
		sim_sm->SendImmediate(loaded_msg);
	}

	void OSGSimpleGraphicsSceneManager::OnSceneShutdown()
	{

	}

	void OSGSimpleGraphicsSceneManager::UpdateFogSettings()
	{
		m_Fog->setColor(osg::Vec4(static_cast<float>(m_FogColor.r), static_cast<float>(m_FogColor.g), static_cast<float>(m_FogColor.b),1));
		m_Fog->setDensity(m_FogDensity);
		m_Fog->setEnd(m_FogEnd);
		m_Fog->setStart(m_FogStart);

		osg::StateSet* state = m_RootNode ? m_RootNode->getOrCreateStateSet() : nullptr;

		const std::string fog_mode_str = "OSG_FOG_MODE";

		if(state)
		{
			short attr = osg::StateAttribute::ON;
			state->setMode(GL_FOG, attr);
		}
		
		
		switch(m_FogMode.GetValue())
		{
		case FM_LINEAR:
			m_Fog->setMode(osg::Fog::LINEAR);
			if(state) state->setDefine(fog_mode_str, "1");
			break;
		case FM_EXP:
			m_Fog->setMode(osg::Fog::EXP);
			if (state) state->setDefine(fog_mode_str, "2");
			break;
		case FM_EXP2:
			m_Fog->setMode(osg::Fog::EXP2);
			if (state) state->setDefine(fog_mode_str, "3");
			break;
		case FM_NONE:
			if(state)
			{
				short attr = osg::StateAttribute::OFF;
				state->setMode(GL_FOG, attr);
				m_RootNode->setStateSet(state);
				state->removeDefine(fog_mode_str);
			}
			break;
		}
	}

	void OSGSimpleGraphicsSceneManager::DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &end_color)
	{
		m_DebugDraw->DrawLine(start_point, end_point, start_color, end_color);
	}

	void OSGSimpleGraphicsSceneManager::OnUpdate(double /*delta_time*/)
	{
		m_DebugDraw->Clear();
	}

	osg::ref_ptr<osgShadow::ShadowedScene> OSGSimpleGraphicsSceneManager::CreateShadowNode()
	{
		osg::ref_ptr<osgShadow::ViewDependentShadowMap> vdsm = new osgShadow::ViewDependentShadowMapExt;
		osg::ref_ptr<osgShadow::ShadowedScene> ss = new osgShadow::ShadowedScene;
		ss->setName("ShadowRootNode");
		ss->setShadowTechnique(vdsm);
	
		osgShadow::ShadowSettings* settings = ss->getShadowSettings();
		settings->setReceivesShadowTraversalMask(NM_RECEIVE_SHADOWS | 0x1);
		settings->setCastsShadowTraversalMask(NM_CAST_SHADOWS);
		settings->setMaximumShadowMapDistance(m_ShadowMaxFarDistance);
		settings->setComputeNearFarModeOverride(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		//settings->setShaderHint(osgShadow::ShadowSettings::PROVIDE_VERTEX_AND_FRAGMENT_SHADER);
		settings->setShaderHint(osgShadow::ShadowSettings::NO_SHADERS);
		//settings->setShadowMapProjectionHint(osgShadow::ShadowSettings::ORTHOGRAPHIC_SHADOW_MAP);
		settings->setMinimumShadowMapNearFarRatio(m_ShadowMinimumNearFarRatio);
		unsigned int num_shadow_maps = 2;
		unsigned int unit = 6;
		settings->setBaseShadowTextureUnit(unit);
		settings->setNumShadowMapsPerLight(num_shadow_maps);
		settings->setMultipleShadowMapHint(osgShadow::ShadowSettings::CASCADED);
		settings->setTextureSize(osg::Vec2s(m_ShadowTextureSize, m_ShadowTextureSize));
		//shader hint
		std::stringstream num_shadow_maps_ss;
		num_shadow_maps_ss << num_shadow_maps;
		ss->getOrCreateStateSet()->setDefine("OSG_NUM_SHADOW_MAPS", num_shadow_maps_ss.str());
		
		ss->getOrCreateStateSet()->addUniform(new osg::Uniform("osg_ShadowTextureUnit0", int(unit)));
		ss->getOrCreateStateSet()->addUniform(new osg::Uniform("osg_ShadowTexture0", int(unit)));
		ss->getOrCreateStateSet()->addUniform(new osg::Uniform("osg_ShadowTextureUnit1", int(unit + 1)));
		ss->getOrCreateStateSet()->addUniform(new osg::Uniform("osg_ShadowTexture1", int(unit + 1)));
		ss->getOrCreateStateSet()->addUniform(new osg::Uniform("osg_ReceiveShadow", bool(true)));
		
		m_ShadowMaxDistanceUniform = new osg::Uniform("osg_ShadowMaxDistance", osg::Vec2f(m_ShadowMaxFarDistance, m_ShadowMaxFarDistance * m_ShadowFadeDistanceRatio));
		ss->getOrCreateStateSet()->addUniform(m_ShadowMaxDistanceUniform);

		m_ShadowSoftnessUniform = new osg::Uniform("osg_ShadowSoftness", m_ShadowSoftness);
		ss->getOrCreateStateSet()->addUniform(m_ShadowSoftnessUniform);
		return ss;
	}
}

