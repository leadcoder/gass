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

#include <math.h>
#include "Core/Math/Polygon.h"
#include "Core/Math/CoreMath.h"

namespace GASS
{

	Polygon::Polygon()
	{

	}

	Polygon::~Polygon()
	{

	}

	void Polygon::Clear()
	{
		m_VertexVector.clear();
	}

	Vec3 Polygon::Center() const
	{
		Vec3 ret;
		ret.Set(0,0,0);
		for(int i = 0; i < m_VertexVector.size(); i++)
		{
			const Vec3* pos = &m_VertexVector[i];
			ret.x += pos->x;
			ret.y += pos->y;
			ret.z += pos->z;
		}
		ret = ret * (1.0/m_VertexVector.size());
		return ret;
	}


	void Polygon::HelpSphereSprite(Vec3 &pos)
	{
		Vec3 face_to_cam,face_to_cam_proj;
		Vec3 cam_pos;// = SimEngine::GetPtr()->m_API.m_CurrentViewport->m_Camera->m_Position;
		Vec3 face_center;
		Vec3 face_normal;
		face_center = Center();
		face_normal  = -m_Normal;

		face_to_cam = cam_pos - face_center;
		face_to_cam_proj = face_to_cam;
		face_to_cam_proj.y = 0;
		face_to_cam.Normalize();
		face_to_cam_proj.Normalize();
		Vec3 upAux = Math::Cross(face_normal,face_to_cam_proj);

		float angleCosine = Math::Dot(face_normal,face_to_cam_proj);
		float h = acos(angleCosine);
		if(upAux.y < 0) h = -h;

		angleCosine = Math::Dot(face_to_cam ,face_to_cam_proj);
		float p = acos(angleCosine);
		if (face_to_cam.y > 0) p= -p;

		Mat4 mat_h;
		Mat4 mat_p;

		//mat.Rotate(h,p,0);
		mat_h.RotateY(h);
		mat_p.RotateX(p);

		pos = (mat_p*mat_h)*pos;
		//pos = mat*pos;
	}
}
