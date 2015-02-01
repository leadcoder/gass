#include "common.shader"


void AmbientVert(float4 position : POSITION,
						  float2 uv		  : TEXCOORD0,
#ifdef VERTEX_COLOR					
						  float4 color : COLOR,
#endif						
						  out float4 oPosition : POSITION,
						  out float4 out_color    : COLOR,

						  uniform float4x4 worldViewProj,
						  uniform float4 ambient)
{
	oPosition = mul(worldViewProj, position);
	
#ifdef VERTEX_COLOR					
	ambient = ambient*color;
#endif
	out_color = ambient;
}


void AmbientTextureVert(float4 position : POSITION,
                          float2 uv          : TEXCOORD0,
#ifdef VERTEX_COLOR					
						  float4 color : COLOR,
#endif
                          out float4 oPosition : POSITION,
                          out float2 oUv       : TEXCOORD0,
#ifdef STD_FOG
						  uniform float3 eyePosition, 
						  out float2 oEyeDist       : TEXCOORD1,
#endif	
                          out float4 out_color    : COLOR,
 
                          uniform float4x4 worldViewProj,
                          uniform float4 ambient)
{
    oPosition = mul(worldViewProj, position);
    oUv = uv;
#ifdef VERTEX_COLOR					
	ambient = ambient*color;
#endif
	out_color = ambient;
#ifdef STD_FOG
   float3 eyeDir = eyePosition.xyz - position.xyz; 
   oEyeDist.x =  length(eyeDir);
#endif	
}


#define BASE_MAP_REG s0
#define DETAIL1_REG s1
#define DETAIL2_REG s2
#define DETAIL3_REG s3
#define DETAIL4_REG s4

void AmbientFrag(float4 ambient : COLOR,
#if BASE_MAP
				float2 uv  : TEXCOORD0,
				uniform sampler2D baseMap : register(BASE_MAP_REG),
#endif
#if DETAIL_SPLATTING
				uniform float4 splatScales,
				uniform sampler2D coverageMap: register(DETAIL1_REG),
				uniform sampler2D splat1Map: register(DETAIL2_REG),
				uniform sampler2D splat2Map: register(DETAIL3_REG),
				uniform sampler2D splat3Map: register(DETAIL4_REG),
#endif
#ifdef STD_FOG
				uniform float3 fogColor,
				uniform float4 fogParams, 
				float2 eyeDist  : TEXCOORD1,
#endif
				out float4 oColor : COLOR)
{
#if BASE_MAP
    float4 base_tex = tex2D(baseMap, uv);
	ambient.a = base_tex.a*ambient.a;
	ambient.xyz = base_tex.xyz * ambient.xyz;
#endif	
#if DETAIL_SPLATTING
    ambient.xyz = calcSplatting(coverageMap,splat1Map,splat2Map,splat3Map,uv,splatScales,ambient.xyz);
#endif	
    
#ifdef STD_FOG
	ambient.xyz = calcFog(eyeDist,fogParams, fogColor, ambient.xyz);
#endif
	ambient.xyz =  ambient.xyz;
	oColor = ambient;
}
 
 
