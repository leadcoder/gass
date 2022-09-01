/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include <memory>

#include "Sim/GASSScene.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGViewport.h"
#include "Plugins/OSG/OSGDebugDraw.h"
#include "Plugins/OSG/Utils/TextBox.h"
#include "Plugins/OSG/Utils/ViewDependentShadowMapExt.h"
#include "Plugins/OSG/OSGMaterial.h"



namespace GASS
{
	OSGGraphicsSceneManager::OSGGraphicsSceneManager(SceneWeakPtr scene):  Reflection(scene), 
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
	

	void OSGGraphicsSceneManager::OnPostConstruction()
	{
		RegisterForPostUpdate<OSGGraphicsSystem>();

		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		ScenePtr scene = GetScene();
		if (!scene)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Scene not present", "OSGGraphicsSceneManager::OnInitialize");
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

		const bool enable_shader_light = true;
		m_RootNode->getOrCreateStateSet()->setDefine("OSG_LIGHTING", enable_shader_light ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
	}

	OSGGraphicsSceneManager::~OSGGraphicsSceneManager(void)
	{
		if(m_ShadowedScene.valid())
			m_ShadowedScene->setShadowTechnique(nullptr);
	}

	void OSGGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register<OSGGraphicsSceneManager>("OSGGraphicsSceneManager");
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("OSG Scene Manager", OF_VISIBLE));
		RegisterGetSet("FogMode", &OSGGraphicsSceneManager::GetFogMode, &OSGGraphicsSceneManager::SetFogMode, PF_VISIBLE | PF_EDITABLE, "Fog type");
		RegisterGetSet( "FogStart", &OSGGraphicsSceneManager::GetFogStart, &OSGGraphicsSceneManager::SetFogStart,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet( "FogEnd", &OSGGraphicsSceneManager::GetFogEnd, &OSGGraphicsSceneManager::SetFogEnd,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet( "FogDensity", &OSGGraphicsSceneManager::GetFogDensity, &OSGGraphicsSceneManager::SetFogDensity,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet( "FogColor", &OSGGraphicsSceneManager::GetFogColor, &OSGGraphicsSceneManager::SetFogColor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet( "AmbientColor", &OSGGraphicsSceneManager::GetAmbientColor, &OSGGraphicsSceneManager::SetAmbientColor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ShadowMaxFarDistance", &OSGGraphicsSceneManager::GetShadowMaxFarDistance, &OSGGraphicsSceneManager::SetShadowMaxFarDistance, PF_VISIBLE | PF_EDITABLE, "");
		RegisterGetSet("ShadowFadeDistanceRatio", &OSGGraphicsSceneManager::GetShadowFadeDistanceRatio, &OSGGraphicsSceneManager::SetShadowFadeDistanceRatio, PF_VISIBLE | PF_EDITABLE, "");
		RegisterGetSet("ShadowMinimumNearFarRatio", &OSGGraphicsSceneManager::GetShadowMinimumNearFarRatio, &OSGGraphicsSceneManager::SetShadowMinimumNearFarRatio, PF_VISIBLE | PF_EDITABLE, "");
		RegisterMember("ShadowTextureSize", &OSGGraphicsSceneManager::m_ShadowTextureSize, PF_VISIBLE, "");
		RegisterGetSet("ShadowSoftness", &OSGGraphicsSceneManager::GetShadowSoftness, &OSGGraphicsSceneManager::SetShadowSoftness, PF_VISIBLE, "");
		RegisterGetSet("ShadowEnabled", &OSGGraphicsSceneManager::GetEnableShadows, &OSGGraphicsSceneManager::SetEnableShadows, PF_VISIBLE | PF_EDITABLE, "");
	}

	float OSGGraphicsSceneManager::GetShadowMaxFarDistance() const
	{
		return m_ShadowMaxFarDistance;
	}
	
	void OSGGraphicsSceneManager::SetShadowMaxFarDistance(float value)
	{
		m_ShadowMaxFarDistance = value;
		if (m_ShadowedScene)
		{
			m_ShadowedScene->getShadowSettings()->setMaximumShadowMapDistance(value);
			if(m_ShadowMaxDistanceUniform)
				m_ShadowMaxDistanceUniform->set(osg::Vec2f(m_ShadowMaxFarDistance, m_ShadowMaxFarDistance * m_ShadowFadeDistanceRatio));
		}
	}

	float OSGGraphicsSceneManager::GetShadowSoftness() const
	{
		return m_ShadowSoftness;
	}

	void OSGGraphicsSceneManager::SetShadowSoftness(float value)
	{
		m_ShadowSoftness = value;
		if (m_ShadowSoftnessUniform)
			m_ShadowSoftnessUniform->set(m_ShadowSoftness);
	}

	float OSGGraphicsSceneManager::GetShadowFadeDistanceRatio() const
	{
		return m_ShadowFadeDistanceRatio;
	}

	void OSGGraphicsSceneManager::SetShadowFadeDistanceRatio(float value)
	{
		m_ShadowFadeDistanceRatio = value;
		if (m_ShadowedScene)
		{
			if (m_ShadowMaxDistanceUniform)
				m_ShadowMaxDistanceUniform->set(osg::Vec2f(m_ShadowMaxFarDistance, m_ShadowMaxFarDistance * m_ShadowFadeDistanceRatio));
		}
	}

	float OSGGraphicsSceneManager::GetShadowMinimumNearFarRatio() const
	{
		return m_ShadowMinimumNearFarRatio;
	}

	bool OSGGraphicsSceneManager::GetEnableShadows() const
	{
		return m_EnableShadows;
	}

	void OSGGraphicsSceneManager::SetEnableShadows(bool value)
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
			}
			else
			{
				if (!m_RootNode->containsNode(m_ShadowRootNode))
					m_RootNode->addChild(m_ShadowRootNode);
				if(m_ShadowedScene && m_ShadowedScene->containsNode(m_ShadowRootNode))
					m_ShadowedScene->removeChild(m_ShadowRootNode);
			}
		}
	}

	void OSGGraphicsSceneManager::SetShadowMinimumNearFarRatio(float value)
	{
		m_ShadowMinimumNearFarRatio = value;
		if (m_ShadowedScene)
			m_ShadowedScene->getShadowSettings()->setMinimumShadowMapNearFarRatio(value);
	}

	void OSGGraphicsSceneManager::SetAmbientColor(const ColorRGB& value)
	{ 
		m_AmbientColor = value;
		if (m_LightModel)
			m_LightModel->setAmbientIntensity(osg::Vec4(static_cast<float>(m_AmbientColor.r), 
				static_cast<float>(m_AmbientColor.g), 
				static_cast<float>(m_AmbientColor.b)
				,1.0f));
	}

	void OSGGraphicsSceneManager::OnSceneCreated()
	{
		void* root = static_cast<void*>(m_RootNode.get());
		void* shadow_node = static_cast<void*>(GetOSGShadowRootNode().get());

		SystemMessagePtr loaded_msg(new GraphicsSceneManagerLoadedEvent(std::string("OSG"),root,shadow_node));
		SimSystemManagerPtr sim_sm = OSGGraphicsSystemPtr(m_GFXSystem)->GetSimSystemManager();
		sim_sm->SendImmediate(loaded_msg);
	}

	void OSGGraphicsSceneManager::OnSceneShutdown()
	{

	}

	void OSGGraphicsSceneManager::UpdateFogSettings()
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

	void OSGGraphicsSceneManager::DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &end_color)
	{
		m_DebugDraw->DrawLine(start_point, end_point, start_color, end_color);
	}

	void OSGGraphicsSceneManager::OnUpdate(double delta_time)
	{
		m_DebugDraw->Clear();
	}

#if 1
	osg::ref_ptr<osgShadow::ShadowedScene> OSGGraphicsSceneManager::CreateShadowNode()
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

#else
	osg::ref_ptr<osgShadow::ShadowedScene> OSGGraphicsSceneManager::_CreateShadowNode() const
	{
		osg::ref_ptr<osgShadow::MinimalShadowMap> sm = NULL;
		//sm = new osgShadow::LightSpacePerspectiveShadowMapDB;
		//sm = new osgShadow::LightSpacePerspectiveShadowMapCB;
		sm = new osgShadow::LightSpacePerspectiveShadowMapVB;
		float minLightMargin(20);
		float maxFarPlane(400);
		short textureSize(2048);
		int baseTextureUnit(0);
		int shadowTextureUnit(6);

		sm->setMinLightMargin(minLightMargin);
		sm->setMaxFarPlane(maxFarPlane);
		sm->setTextureSize(osg::Vec2s(static_cast<short>(textureSize), static_cast<short>(textureSize)));
		sm->setShadowTextureCoordIndex(shadowTextureUnit);
		sm->setShadowTextureUnit(shadowTextureUnit);
		sm->setBaseTextureCoordIndex(baseTextureUnit);
		sm->setBaseTextureUnit(baseTextureUnit);
		//sm->setShadowReceivingCoarseBoundAccuracy(osgShadow::MinimalShadowMap::EMPTY_BOX);
		sm->setMainVertexShader(NULL);
		sm->setShadowVertexShader(NULL);

		osg::Shader* mainFragmentShader = new osg::Shader(osg::Shader::FRAGMENT,
			" // following expressions are auto modified - do not change them:       \n"
			" // gl_TexCoord[0]  0 - can be subsituted with other index              \n"
			"                                                                        \n"
			"float DynamicShadow( );                                                 \n"
			"                                                                        \n"
			"uniform sampler2D baseTexture;                                          \n"
			"                                                                        \n"
			"void main(void)                                                         \n"
			"{                                                                       \n"
			"  vec4 colorAmbientEmissive = gl_FrontLightModelProduct.sceneColor;     \n"
			"  // Add ambient from Light of index = 0                                \n"
			"  colorAmbientEmissive += gl_FrontLightProduct[0].ambient;              \n"
			"  vec4 color = texture2D( baseTexture, gl_TexCoord[0].xy );             \n"
			"  color *= mix( colorAmbientEmissive, gl_Color, DynamicShadow() );      \n"
			//"  const float LOG2E = 1.442692;	// = 1/log(2)                        \n"
			//"  float fog = exp2(-gl_Fog.density * abs(gl_FogFragCoord) * LOG2E);     \n"
			//"  fog = clamp(fog, 0.0, 1.0);                                            \n"
			//hack to support linear and exp fog, TODO: add fog mode uniform 
			"  if(gl_Fog.density > 0){                                            \n"
			"    float depth = gl_FragCoord.z / gl_FragCoord.w;\n"
			"    float fogFactor = exp(-pow((gl_Fog.density * depth), 2.0));\n"
			"    fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
			"    color.rgb = mix( gl_Fog.color.rgb, color.rgb, fogFactor );            \n"
			"  } \n"
			"  else { \n"
			"     float fogFactor = clamp((gl_Fog.end - abs(gl_FogFragCoord))*gl_Fog.scale, 0.0,1.0);\n"
			"     color.rgb = mix( gl_Fog.color.rgb, color.rgb, fogFactor );            \n"
			"  } \n"
			"    gl_FragColor = color;                                                 \n"
			"} \n");

		sm->setMainFragmentShader(mainFragmentShader);
		/*osg::Shader* shadowFragmentShader = new osg::Shader( osg::Shader::FRAGMENT,
		" // following expressions are auto modified - do not change them:      \n"
		" // gl_TexCoord[1]  1 - can be subsituted with other index             \n"
		"                                                                       \n"
		"uniform sampler2DShadow shadowTexture;                                 \n"
		"                                                                       \n"
		"float DynamicShadow( )                                                 \n"
		"{                                                                      \n"
		"    return shadow2DProj( shadowTexture, gl_TexCoord[1] ).r;            \n"
		"} \n" );*/
		osg::ref_ptr<osgShadow::ShadowedScene> ss = new osgShadow::ShadowedScene;
		ss->setName("ShadowRootNode");
		osgShadow::ShadowSettings* settings = ss->getShadowSettings();
		settings->setReceivesShadowTraversalMask(NM_RECEIVE_SHADOWS);
		settings->setCastsShadowTraversalMask(NM_CAST_SHADOWS);
		ss->setShadowTechnique(sm);

		ss->getOrCreateStateSet()->setDefine("OSG_NUM_SHADOW_MAPS", "1");
		osg::Uniform* shadowTextureUnitUniform = new osg::Uniform(osg::Uniform::INT, "shadowTextureUnit0");
		shadowTextureUnitUniform->set((int)shadowTextureUnit);
		ss->getOrCreateStateSet()->addUniform(shadowTextureUnitUniform);
		//Add texture sampler and unit uniforms,to match vdms and our shadow shaders
		osg::Uniform* shadowTextureSampler = new osg::Uniform(osg::Uniform::INT, "shadowTexture0");
		shadowTextureSampler->set((int)shadowTextureUnit);
		ss->getOrCreateStateSet()->addUniform(shadowTextureSampler);
		
		return ss;
	}
#endif
}

