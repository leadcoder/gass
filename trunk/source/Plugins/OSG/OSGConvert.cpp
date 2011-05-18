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

#include "OSGConvert.h"

namespace GASS
{
	
	OSGConvert::OSGConvert() :m_FlipYZ (true)
	{
		m_Tranform.Identity();
		m_Tranform.SetUpVector(Vec3(0,0,1));
		m_Tranform.SetRightVector(Vec3(1,0,0));
		m_Tranform.SetViewDirVector(Vec3(0,-1,0));
		m_InvTranform = m_Tranform.Invert();
		
	}

	OSGConvert::~OSGConvert()
	{

	}

	template<> OSGConvert* Singleton<OSGConvert>::m_Instance = 0;
	OSGConvert* OSGConvert::GetPtr(void)
	{
		//assert(m_Instance);
		if(m_Instance == NULL)
			m_Instance = new OSGConvert();
		return m_Instance;
	}

	OSGConvert& OSGConvert::Get(void)
	{
		//assert(m_Instance);
		if(m_Instance == NULL)
			m_Instance = new OSGConvert();
		return *m_Instance;
	}

	osg::Vec3d OSGConvert::ToOSG(const Vec3 &v) const
	{
		//Vec3 osg_v = m_Tranform*v;
		//return osg::Vec3(osg_v.x,osg_v.y,osg_v.z);
		if(m_FlipYZ)
			return osg::Vec3d(v.x,-v.z,v.y);
		else
			return osg::Vec3d(v.x,v.y,v.z);
	}

	Vec3 OSGConvert::ToGASS(const osg::Vec3 &v) const
	{
		if(m_FlipYZ)
			return Vec3(v.x(),v.z(),-v.y());
		else
			return Vec3(v.x(),v.y(),v.z());

	}

	Vec3 OSGConvert::ToGASS(const osg::Vec3d &v) const
	{
		if(m_FlipYZ)
			return Vec3(v.x(),v.z(),-v.y());
		else
			return Vec3(v.x(),v.y(),v.z());
	}

	Quaternion OSGConvert::ToGASS(const osg::Quat &value) const
	{
		if(m_FlipYZ)
			return Quaternion(value.w(),-value.x(),-value.z(),value.y());
		else
			return Quaternion(-value.w(),value.x(), value.y(), value.z());
		
	/*	Mat4 rot_mat;
		rot_mat.Identity();
		Quaternion rot(-value.w(),value.x(),value.y(),value.z());
		
		rot.ToRotationMatrix(rot_mat);

		Vec3 view = rot_mat.GetViewDirVector();
		Vec3 up = rot_mat.GetUpVector();
		Vec3 right = rot_mat.GetRightVector();

		rot_mat = m_InvTranform*rot_mat;

		view = rot_mat.GetViewDirVector();
		up = rot_mat.GetUpVector();
		right = rot_mat.GetRightVector();

		rot.FromRotationMatrix(rot_mat);
		return rot;*/

		//return Quaternion(-value.w(),value.x(),value.y(),value.z());
		osg::Matrixf osg_matrix;
		value.get(osg_matrix);
		float test = osg_matrix(0,0);
	
		Vec3 right_dir = ToGASS(osg::Vec3(osg_matrix(0,0),osg_matrix(0,1),osg_matrix(0,2)));
		Vec3 view_dir = ToGASS(osg::Vec3(osg_matrix(1,0),osg_matrix(1,1),osg_matrix(1,2)));
		Vec3 up_dir = ToGASS(osg::Vec3(osg_matrix(2,0),osg_matrix(2,1),osg_matrix(2,2)));
		
		Mat4 rot_mat;
		rot_mat.Identity();
		rot_mat.SetViewDirVector(-view_dir);
		rot_mat.SetUpVector(up_dir);
		rot_mat.SetRightVector(right_dir);
		
		Quaternion rot;
		rot.FromRotationMatrix(rot_mat);
		return rot;
	}

	osg::Quat OSGConvert::ToOSG(const Quaternion &value) const
	{
		if(m_FlipYZ)
			return  osg::Quat(-value.x,value.z,-value.y,value.w);
		else
			return  osg::Quat(value.x,value.y,value.z,-value.w);

		/*Mat4 rot_mat;
		Quaternion rot = value;
		rot.ToRotationMatrix(rot_mat);
		rot_mat = m_Tranform*rot_mat;
		rot.FromRotationMatrix(rot_mat);
		return osg::Quat(rot.x,rot.y,rot.z,-rot.w);*/

		//return  osg::Quat(value.x,value.y,value.z,-value.w);
		Mat4 rot_mat;
		value.ToRotationMatrix(rot_mat);

		osg::Vec3 view = -ToOSG(rot_mat.GetViewDirVector());
		osg::Vec3 up = ToOSG(rot_mat.GetUpVector());
		osg::Vec3 right = ToOSG(rot_mat.GetRightVector());

		osg::Matrixf osg_matrix;
		osg_matrix.makeIdentity();

		//right_dir
		osg_matrix(0,0) = right.x();
		osg_matrix(0,1) = right.y();
		osg_matrix(0,2) = right.z();

		//view_dir
		osg_matrix(1,0) = view.x();
		osg_matrix(1,1) = view.y();
		osg_matrix(1,2) = view.z();

		//up_dir
		osg_matrix(2,0) = up.x();
		osg_matrix(2,1) = up.y();
		osg_matrix(2,2) = up.z();

		osg::Quat rot;
		rot = osg_matrix.getRotate();


		//std::cout << value.w << " " << -value.x << " " << value.z << " " << -value.y << "\n";
		//std::cout << rot.w() << " " << rot.x() << " " << rot.y() << " " << rot.z() << "\n";

		return rot;

	}
}



