R"(
 #version 400 compatibility
 #pragma import_defines (OSG_LIGHTING, OSG_NUM_SHADOW_MAPS)
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ModelViewProjectionMatrix;
uniform mat3 osg_NormalMatrix;

uniform int baseTextureUnit;
uniform int osg_ShadowTextureUnit0;
uniform int osg_ShadowTextureUnit1;

out osg_VertexData
{
  vec4 Position;
  vec4 ModelViewPosition;
  vec3 Normal;
  vec2 TexCoord0;
} osg_out;

//layout(location = 2) in vec4 color_in;

void setShadowTexCoords(vec4 mv_pos)
{
	//generate coords for shadow mapping
#if (OSG_NUM_SHADOW_MAPS > 0)
	gl_TexCoord[osg_ShadowTextureUnit0].s = dot(mv_pos, gl_EyePlaneS[osg_ShadowTextureUnit0]);
	gl_TexCoord[osg_ShadowTextureUnit0].t = dot(mv_pos, gl_EyePlaneT[osg_ShadowTextureUnit0]);
	gl_TexCoord[osg_ShadowTextureUnit0].p = dot(mv_pos, gl_EyePlaneR[osg_ShadowTextureUnit0]);
	gl_TexCoord[osg_ShadowTextureUnit0].q = dot(mv_pos, gl_EyePlaneQ[osg_ShadowTextureUnit0]);
#if (OSG_NUM_SHADOW_MAPS > 1)
	gl_TexCoord[osg_ShadowTextureUnit1].s = dot(mv_pos, gl_EyePlaneS[osg_ShadowTextureUnit1]);
	gl_TexCoord[osg_ShadowTextureUnit1].t = dot(mv_pos, gl_EyePlaneT[osg_ShadowTextureUnit1]);
	gl_TexCoord[osg_ShadowTextureUnit1].p = dot(mv_pos, gl_EyePlaneR[osg_ShadowTextureUnit1]);
	gl_TexCoord[osg_ShadowTextureUnit1].q = dot(mv_pos, gl_EyePlaneQ[osg_ShadowTextureUnit1]);
#endif
#endif
}

vec4 getDirectionalLight(int index, vec3 normal)
{
	vec3 light_dir = normalize(gl_LightSource[index].position.xyz);
	float NdotL = max(dot(normal, light_dir), 0.0);
 	vec4 color = min(NdotL * gl_FrontLightProduct[index].diffuse + gl_FrontLightProduct[index].ambient, 1.0);
	if (NdotL > 0.0)
	{
		float NdotHV = max(0.0, dot(normal, vec3(gl_LightSource[index].halfVector)));
		float pf = pow(NdotHV, gl_FrontMaterial.shininess);
		color += gl_FrontLightProduct[index].specular * pf;
	}
	return color;
}

void main()
{
	osg_out.TexCoord0 = gl_MultiTexCoord0.xy;
	osg_out.Position = gl_Vertex;
	osg_out.Normal = normalize(osg_NormalMatrix * gl_Normal);
	gl_Position = osg_ModelViewProjectionMatrix * osg_out.Position;
	osg_out.ModelViewPosition = osg_ModelViewMatrix * osg_out.Position;
 	vec4 color = gl_Color * getDirectionalLight(0, osg_out.Normal);
    gl_FrontColor = color;
	setShadowTexCoords(osg_out.ModelViewPosition);
}
)"