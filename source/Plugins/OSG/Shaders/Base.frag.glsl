R"(
#version 400 compatibility
#pragma import_defines (OSG_LIGHTING, OSG_FOG_MODE, OSG_NUM_SHADOW_MAPS, OSG_ALBEDO_MAP, OSG_NORMAL_MAP, OSG_RECEIVESHADOWS, OSG_IS_SHADOW_CAMERA)


#ifdef OSG_ALBEDO_MAP
	uniform sampler2D osg_AlbedoMap;
#endif

#ifdef OSG_NORMAL_MAP
	uniform sampler2D osg_NormalMap;
#endif

uniform sampler2DShadow osg_ShadowTexture0;
uniform sampler2DShadow osg_ShadowTexture1;
uniform vec2 osg_ShadowMaxDistance;
uniform float osg_ShadowSoftness;

uniform mat4 osg_ModelViewMatrix;
in vec4 gass_ShadowTexCoord[2];

in osg_VertexData
{
  vec4 Position;
  vec4 ModelViewPosition;
  vec3 Normal;
  vec2 TexCoord0;
  vec4 Color;
} osg_in;

float getPCFShadowMapValue(sampler2DShadow shadowmap, vec4 shadowUV)
{
	// PCF filtering
	ivec2 tex_size = textureSize(shadowmap,0);
	float invTexel = 1.0 / float(tex_size.x);
	float offset  = osg_ShadowSoftness * invTexel * shadowUV.w;
	float shadowTerm = shadow2DProj(shadowmap, shadowUV).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV - vec4(offset, 0.0, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV + vec4(offset, 0.0, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV - vec4(0.0, offset, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV + vec4(0.0, offset, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV - vec4(offset, offset, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV + vec4(offset, offset, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV - vec4(offset,-offset, 0.0, 0.0)).r;
	shadowTerm += shadow2DProj(shadowmap, shadowUV + vec4(offset,-offset, 0.0, 0.0)).r;
	shadowTerm = shadowTerm / 9.0;
	return shadowTerm;
}

float getShadowMapValue(sampler2DShadow shadowmap, vec4 shadowUV, float bias)
{
	shadowUV.z -= bias;
	if(osg_ShadowSoftness > 0.0)
		return getPCFShadowMapValue(shadowmap, shadowUV);
	else
		return shadow2DProj(shadowmap, shadowUV).r;
}

float getShadowFactor(vec3 normal, float depth)
{
	const float b0 = 0.00001;
	vec3 L = normalize(gl_LightSource[0].position.xyz);
    vec3 N = normalize(normal);
    float costheta = clamp(dot(L,N), 0.0, 1.0);
    float bias = b0*tan(acos(costheta));

	float shadow = 1.0;
#ifdef OSG_NUM_SHADOW_MAPS
#if (OSG_NUM_SHADOW_MAPS > 0)
	shadow *= getShadowMapValue(osg_ShadowTexture0, gass_ShadowTexCoord[0], bias);
#if (OSG_NUM_SHADOW_MAPS > 1)
	shadow *= getShadowMapValue(osg_ShadowTexture1, gass_ShadowTexCoord[1], bias);
#endif
#endif
#endif
	float shadow_fade = min( max(osg_ShadowMaxDistance.x - depth, 0.0) / osg_ShadowMaxDistance.y, 1.0);
	return mix(1.0, shadow, shadow_fade);
}

vec3 applyFog(vec3 color, float depth)
{
#ifdef OSG_FOG_MODE
#if OSG_FOG_MODE == 1 //LINEAR
    float fog_factor = (gl_Fog.end - depth) * gl_Fog.scale;
#elif OSG_FOG_MODE == 2 //EXP
    float fog_factor = exp(-gl_Fog.density * depth);
#elif OSG_FOG_MODE == 3 //EXP2
    float fog_factor = exp(-pow((gl_Fog.density * depth), 2.0));
#endif
    fog_factor = clamp(fog_factor, 0.0, 1.0);
    color.xyz = mix(gl_Fog.color.xyz, color.xyz, fog_factor);
#endif
	return color;
}

vec4 getDirectionalLight(int index, vec3 normal)
{
	vec3 light_dir = normalize(gl_LightSource[index].position.xyz);
	float NdotL = max(dot(normal, light_dir), 0.0);
 	vec4 color = max(vec4(0), min(NdotL * gl_FrontLightProduct[index].diffuse + gl_FrontLightProduct[index].ambient, 1.0));
	if (NdotL > 0.0)
	{
		float NdotHV = max(0.0, dot(normal, vec3(gl_LightSource[index].halfVector)));
		float pf = pow(NdotHV, gl_FrontMaterial.shininess);
		color += gl_FrontLightProduct[index].specular * pf;
	}
	return color;
}

vec3 getNormal()
{
#ifdef OSG_NORMAL_MAP
	vec3 tangentNormal = normalize(texture(osg_NormalMap, osg_in.TexCoord0.xy).xyz * 2.0 - 1.0);
	vec3 p_dx  = dFdx(osg_in.ModelViewPosition.xyz);
	vec3 p_dy  = dFdy(osg_in.ModelViewPosition.xyz);
	vec2 tc_dx = dFdx(osg_in.TexCoord0.xy);
	vec2 tc_dy = dFdy(osg_in.TexCoord0.xy);
	vec3 N = normalize(osg_in.Normal);
	vec3 T = normalize(tc_dy.y * p_dx - tc_dx.y * p_dy);
	vec3 B = normalize(tc_dy.x * p_dx - tc_dx.x * p_dy); // sign inversion
	//vec3 B  = -normalize(cross(N, T));
	// get new tangent from a given mesh normal
	vec3 x = cross(N, T);
	T = normalize(cross(x, N));
	// get updated bi-tangent
	x = cross(B, N);
	B = normalize(cross(N, x));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNormal);
#else
	return osg_in.Normal;
#endif
}

void main(void)
{
#ifdef OSG_IS_SHADOW_CAMERA
	gl_FragColor = vec4(1,1,1,1);
#else
	vec4 color = osg_in.Color;
#ifdef OSG_ALBEDO_MAP
	color *= texture2D(osg_AlbedoMap, osg_in.TexCoord0.xy);
#endif

	float depth = length(osg_in.ModelViewPosition);

#ifdef OSG_LIGHTING
	vec3 normal = getNormal();
	vec4 lit_color = getDirectionalLight(0, normal);
    vec4 colorAmbientEmissive = gl_FrontLightModelProduct.sceneColor;
#ifdef OSG_RECEIVESHADOWS
    float shadow_factor = getShadowFactor(normal,depth);
    color *= (colorAmbientEmissive + mix(gl_FrontLightProduct[0].ambient, lit_color ,shadow_factor ));
#else
	color *= (colorAmbientEmissive + lit_color);
#endif
#endif
	color.xyz = applyFog(color.xyz, depth);
    gl_FragColor = color;
#endif
}
)"