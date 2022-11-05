#pragma once
#include "Modules/Graphics/OSGCommon.h"

namespace GASS
{

	//From Moon
	template< class T >
	class DrawableVisitor : public osg::NodeVisitor
	{
	public:
		bool m_GetGeode;
		osg::TriangleFunctor<T> mFunctor;

		/**
		* Constructor.
		*/
		DrawableVisitor()
			: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN),m_GetGeode(true)
		{}

		/**
		* Applies this visitor to a geode.
		*
		* @param node the geode to visit
		*/
		virtual void apply(osg::Geode& node)
		{
			if(!m_GetGeode)
				return;
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

		virtual void apply(osg::PagedLOD& plod)
		{
			m_GetGeode = true;
			if (plod.getNumFileNames()>0)
			{
				// Identify the range value for the highest res child
				float targetRangeValue;
				if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
					targetRangeValue = 1e6; // Init high to find min value
				else
					targetRangeValue = 0; // Init low to find max value

				const osg::LOD::RangeList rl = plod.getRangeList();
				osg::LOD::RangeList::const_iterator rit;
				for( rit = rl.begin();
					rit != rl.end();
					++rit)
				{
					if( plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT )
					{
						if( rit->first < targetRangeValue )
							targetRangeValue = rit->first;
					}
					else
					{
						if( rit->first > targetRangeValue )
							targetRangeValue = rit->first;
					}
				}

				// Perform an intersection test only on children that display
				// at the maximum resolution.
				unsigned int childIndex;
				for( rit = rl.begin(), childIndex = 0;
					rit != rl.end();
					++rit, childIndex++ )
				{
					if( rit->first != targetRangeValue )
						// This is not one of the highest res children
						continue;

					osg::ref_ptr<osg::Node> child( NULL );
					if( plod.getNumChildren() > childIndex )
						child = plod.getChild( childIndex );

					if( (!child.valid()))
					{
						// Child is NULL; attempt to load it, if we have a readCallback...
						unsigned int validIndex( childIndex );
						if (plod.getNumFileNames() <= childIndex)
							validIndex = plod.getNumFileNames()-1;

						
						const std::string filename = plod.getFileName(validIndex);
						if (filename != "")
						{
							m_GetGeode = false; //disable geometry fetch for this PagedLOD node, this is not a high resolution node
							const std::string plod_file = plod.getDatabasePath() + filename;
							child = osgDB::readNodeFile(plod_file);
						}
					}

					if ( !child.valid() && plod.getNumChildren()>0)
					{
						// Child is still NULL, so just use the one at the end of the list.
						child = plod.getChild( plod.getNumChildren()-1 );
					}

					if (child.valid())
					{
						child->accept(*this);
					}
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
			const osg::Vec3& v3
#ifdef OSG_VERSION_LESS_THAN
#if(OSG_VERSION_LESS_THAN(3,6,0))
			, bool treatVertexDataAsTemporary
#endif
#else
			, bool treatVertexDataAsTemporary
#endif
			)
		{

			/*mMatrix.set(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
			Math::Deg2Rad(180),osg::Vec3(0,1,0),
			Math::Deg2Rad(0),osg::Vec3(0,0,1)));*/
			osg::Vec3d tv1 = v1*mMatrix;
			osg::Vec3d tv2 = v2*mMatrix;
			osg::Vec3d tv3 = v3*mMatrix;

			Vec3 sv1, sv2, sv3;
			unsigned int index = static_cast<unsigned int>(mVertices.size());

			mTriangles.push_back(index+0);
			mTriangles.push_back(index+1);
			mTriangles.push_back(index+2);

			sv1 = OSGConvert::ToGASS(tv1);
			sv2 = OSGConvert::ToGASS(tv2);
			sv3 = OSGConvert::ToGASS(tv3);

			mVertices.push_back(sv1);
			mVertices.push_back(sv2);
			mVertices.push_back(sv3);
		}
	};
}
