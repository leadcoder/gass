#pragma once   

//mesh fetch
#include <osg/TriangleFunctor>
#include <osg/NodeVisitor>

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
	//struct StridedVertex {
//		Vec3 Vertex;
	//};
	
	/**
	 * A strided triangle for the ODE triangle mesh collision geometry.
	 */
	//struct StridedTriangle {
//		unsigned int Indices[3];
	//};
	
	/**
	 * Records visited triangles into an array suitable for passing
	 * into ODE's triangle mesh collision detection class.
	 */
	class TriangleRecorder {
	public:
		
		std::vector<Vec3> mVertices;
		std::vector<unsigned int> mTriangles;
		
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
			
			Vec3 sv1, sv2, sv3;
			unsigned int index = mVertices.size();

			mTriangles.push_back(index+0);
			mTriangles.push_back(index+1);
			mTriangles.push_back(index+2);
			
			sv1 = OSGConvert::Get().ToGASS(tv1);
			sv2 = OSGConvert::Get().ToGASS(tv2);
			sv3 = OSGConvert::Get().ToGASS(tv3);
			
			mVertices.push_back(sv1);
			mVertices.push_back(sv2);
			mVertices.push_back(sv3);
		}
	};
}