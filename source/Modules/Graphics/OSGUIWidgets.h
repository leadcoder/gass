#pragma once
#include "Sim/GASS.h"
#include <imgui.h>
#include "Modules/Graphics/OSGCommon.h"
#include "Modules/Graphics/OSGConvert.h"

namespace GASS
{
	class MaterialGUIVisitor : public osg::NodeVisitor
	{
		std::set<osg::Material*> Materials;
	public:
		MaterialGUIVisitor()
			: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
		{}
		void apply(osg::Node& node)
		{
			auto state_set = node.getStateSet();
			if (state_set)
			{
				auto material = dynamic_cast<osg::Material*>(state_set->getAttribute(osg::StateAttribute::MATERIAL));
				if (material && Materials.find(material) == Materials.end())
				{
					Materials.insert(material);
					auto name = "osg::Material" + material->getName();
					if (ImGui::TreeNode(name.c_str()))
					{
						ImGui::PushID(material);
						auto a = material->getAmbient(osg::Material::FRONT_AND_BACK);
						if (ImGui::ColorEdit4("Ambient", &a.x()))
							material->setAmbient(osg::Material::FRONT_AND_BACK, a);
						auto d = material->getDiffuse(osg::Material::FRONT_AND_BACK);
						if (ImGui::ColorEdit4("Diffuse", &d.x()))
							material->setDiffuse(osg::Material::FRONT_AND_BACK, d);
						auto s = material->getSpecular(osg::Material::FRONT_AND_BACK);
						if (ImGui::ColorEdit3("Specular", &s.x()))
							material->setSpecular(osg::Material::FRONT_AND_BACK, s);

						auto shininess = material->getShininess(osg::Material::FRONT_AND_BACK);
						if (ImGui::DragFloat("Shininess", &shininess))
							material->setShininess(osg::Material::FRONT_AND_BACK, shininess);

						auto e = material->getEmission(osg::Material::FRONT_AND_BACK);
						if (ImGui::ColorEdit3("Emission", &e.x()))
							material->setEmission(osg::Material::FRONT_AND_BACK, e);
						auto color_mode = (int)material->getColorMode();
						if (ImGui::RadioButton("AMBIENT", &color_mode, osg::Material::AMBIENT))
							material->setColorMode(osg::Material::AMBIENT);
						if (ImGui::RadioButton("DIFFUSE", &color_mode, osg::Material::DIFFUSE))
							material->setColorMode(osg::Material::DIFFUSE);
						if (ImGui::RadioButton("SPECULAR", &color_mode, osg::Material::SPECULAR))
							material->setColorMode(osg::Material::SPECULAR);
						if (ImGui::RadioButton("EMISSION", &color_mode, osg::Material::EMISSION))
							material->setColorMode(osg::Material::EMISSION);
						if (ImGui::RadioButton("AMBIENT_AND_DIFFUSE", &color_mode, osg::Material::AMBIENT_AND_DIFFUSE))
							material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
						if (ImGui::RadioButton("OFF", &color_mode, osg::Material::OFF))
							material->setColorMode(osg::Material::OFF);
						ImGui::PopID();
						ImGui::TreePop();
					}

					auto num_textures = state_set->getTextureAttributeList().size();
					if (num_textures > 0)
					{
						if (ImGui::TreeNode("Textures"))
						{
							for (unsigned int i = 0; i < num_textures; ++i)
							{
								osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(state_set->getTextureAttribute(i, osg::StateAttribute::TEXTURE));
								if (texture)
								{
									std::string tex_name;
									if (texture->getImage())
										tex_name = texture->getImage()->getFileName();
									else
										tex_name = texture->getName();
									ImGui::Text(tex_name.c_str());
								}
							}
							ImGui::TreePop();
						}
					}
				}
			}
		}

		void apply(osg::Group& node)
		{
			traverse(node);
		}
	};
}

