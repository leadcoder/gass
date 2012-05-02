/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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
#pragma once
#   pragma warning (disable : 4541)
#include "Sim/GASS.h"
//mesh fetch
#include <osg/TriangleFunctor>
#include <osg/NodeVisitor>

namespace osg
{
	class Geode;
}
namespace GASS
{
	//From Moon
	template< class T >
	class DrawableVisitor : public osg::NodeVisitor 
	{
	public:
		
		osg::TriangleFunctor<T> mFunctor;
		
		/**
		 * Constructor.
		 */
		DrawableVisitor()
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
		{}
		
		/**
		 * Applies this visitor to a geode.
		 *
		 * @param node the geode to visit
		 */
		virtual void apply(osg::Geode& node)
		{
			for(unsigned int i=0;i<node.getNumDrawables();i++)
			{
				osg::Drawable* d = node.getDrawable(i);
				
				if(d->supports(mFunctor))
				{
					osg::NodePath nodePath = getNodePath();
					mFunctor.mMatrix = osg::computeLocalToWorld(nodePath);					
					d->accept(mFunctor);
				}
			}
		}
	};

	/**
	 * A strided vertex for the ODE triangle mesh collision geometry.
	 */
	struct StridedVertex {
		Vec3 Vertex;
	};
	
	/**
	 * A strided triangle for the ODE triangle mesh collision geometry.
	 */
	struct StridedTriangle {
		unsigned int Indices[3];
	};
	
	/**
	 * Records visited triangles into an array suitable for passing
	 * into ODE's triangle mesh collision detection class.
	 */
	class TriangleRecorder {
	public:
		
		std::vector<StridedVertex> mVertices;
		
		std::vector<StridedTriangle> mTriangles;
		
		osg::Matrixd mMatrix;
		
		/**
		 * Called once for each visited triangle.
		 *
		 * @param v1 the triangle's first vertex
		 * @param v2 the triangle's second vertex
		 * @param v3 the triangle's third vertex
		 * @param treatVertexDataAsTemporary whether or not to treat the vertex data
		 * as temporary
		 */
		void operator()(const osg::Vec3& v1,
						const osg::Vec3& v2,
						const osg::Vec3& v3,
						bool treatVertexDataAsTemporary)
		{

			/*mMatrix.set(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
									     Math::Deg2Rad(180),osg::Vec3(0,1,0),
										 Math::Deg2Rad(0),osg::Vec3(0,0,1)));*/
			osg::Vec3d tv1 = v1*mMatrix;
			osg::Vec3d tv2 = v2*mMatrix;
			osg::Vec3d tv3 = v3*mMatrix;

			//osg::Vec3 tv1 = v1;
			//osg::Vec3 tv2 = v2;
			//osg::Vec3 tv3 = v3;
			
			StridedVertex sv1, sv2, sv3;
			StridedTriangle t;
			
			t.Indices[0] = mVertices.size();
			t.Indices[1] = mVertices.size() + 1;
			t.Indices[2] = mVertices.size() + 2;
			
			mTriangles.push_back(t);
			
			sv1.Vertex = OSGConvert::Get().ToGASS(tv1);
			sv2.Vertex = OSGConvert::Get().ToGASS(tv2);
			sv3.Vertex = OSGConvert::Get().ToGASS(tv3);
			/*sv1.Vertex.x = tv1[0];
			sv1.Vertex.y = tv1[1];
			sv1.Vertex.z = tv1[2];
			
			sv2.Vertex.x = tv2[0];
			sv2.Vertex.y = tv2[1];
			sv2.Vertex.z = tv2[2];
			
			sv3.Vertex.x = tv3[0];
			sv3.Vertex.y = tv3[1];
			sv3.Vertex.z = tv3[2];*/
			
			//Changed winding order
			mVertices.push_back(sv1);
			mVertices.push_back(sv2);
			mVertices.push_back(sv3);
		}
	};
	

	class OSGMeshComponent :	public Reflection<OSGMeshComponent,BaseSceneComponent> , public IMeshComponent , public IGeometryComponent
	{
	public:
		OSGMeshComponent (void);
		~OSGMeshComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();

		//IGeometryComponent
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryCategory GetGeometryCategory() const;

		//IMeshComponent
		virtual std::string GetFilename() const {return m_Filename;}
		virtual void GetMeshData(MeshDataPtr mesh_data) const;

		//set external mesh
		void SetMeshNode(osg::ref_ptr<osg::Group> mesh) {m_MeshNode =mesh;}
	protected:
		void SetGeometryCategory(const GeometryCategory &value);
		bool GetLighting() const;
		void SetLighting(bool value);
		void SetFilename(const std::string &filename);
		
		bool GetCastShadow()const {return m_CastShadow;}
		void SetCastShadow(bool value);
		bool GetReceiveShadow()const {return m_ReceiveShadow;}
		void SetReceiveShadow(bool value);
		void Expand(SceneObjectPtr parent, osg::Group *group, bool load);
		
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnMaterialMessage(MaterialMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnMeshFileNameMessage(MeshFileMessagePtr message);
		void CalulateBoundingbox(osg::Node *node, const osg::Matrix& M = osg::Matrix::identity());
		void LoadMesh(const std::string &filename);

		std::string m_Filename;
		bool m_CastShadow;
		bool m_ReceiveShadow;
		osg::ref_ptr<osg::Group> m_MeshNode;
		AABox m_BBox;
		bool m_Initlized;
		GeometryCategory m_Category;
		bool m_Lighting;
	};
}

