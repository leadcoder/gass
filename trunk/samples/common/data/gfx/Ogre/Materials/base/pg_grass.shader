#include "common.shader"
float3 calc_light(float4 position, float3 surface_diffuse,float4 lightDiffuse, float4 lightPosition, float4 lightAtten,float4 spotParams,float3 spotDir)
{
	float3 lightDir = lightPosition.xyz -  (position.xyz * lightPosition.w);
	float lightDist = length(lightDir);
	lightDir = normalize(lightDir);
	float diffuseFactor = max(0,dot(float3(0.0,1.0,0.0), lightDir));
	float3 lit_diffuse = surface_diffuse*diffuseFactor*lightDiffuse.xyz;
	float rho = saturate(dot(-spotDir, lightDir));
   // factor = (rho - cos(outer/2) / cos(inner/2) - cos(outer/2)) ^ falloff
   float spotFactor = pow(saturate(rho - spotParams.y) / (spotParams.x - spotParams.y), spotParams.z);
   lit_diffuse = spotFactor*lit_diffuse;// + spotFactor*lightSpecular * litRes.z;
   float4 attn = ( 1.0 / (( lightAtten.y ) + ( lightAtten.z * lightDist ) + ( lightAtten.w * lightDist * lightDist )));
   lit_diffuse =  lit_diffuse*attn;
   return lit_diffuse;
}

void pg_grass_fp(
	float2 uv : TEXCOORD0,
	float4 position : TEXCOORD2,
	float4 color : COLOR,
	uniform float4 ambient,
	
	uniform float4 lightDiffuse,
	uniform float4 lightPosition,
	uniform float4 lightAtten,
    uniform float4 spotParams,
	uniform float3 spotDir,
	
	uniform float4 lightDiffuse1,
	uniform float4 lightPosition1,
	uniform float4 lightAtten1,
    uniform float4 spotParams1,
	uniform float3 spotDir1,
	
	uniform sampler2D diffuseMap : register(s0),
#ifdef INTEGRATED_SHADOWS
	float4 shadowUV : TEXCOORD1,
	uniform float4 shadowParams,
	uniform sampler2D shadowMap : register(s1),
#endif	
	out float4 oColour : COLOR)
{
	float4 surface_diffuse = tex2D(diffuseMap, uv)*color;
	float3 lit_diffuse = calc_light(position, surface_diffuse,lightDiffuse, lightPosition, lightAtten, spotParams,spotDir);
#ifdef INTEGRATED_SHADOWS
	float visibility = calcShadow(shadowUV,shadowMap);
	lit_diffuse = lit_diffuse*visibility;
#endif
	lit_diffuse = lit_diffuse + calc_light(position, tex2D(diffuseMap, uv),lightDiffuse1, lightPosition1, lightAtten1, spotParams1,spotDir1);

	oColour = float4(lit_diffuse + ambient.xyz*surface_diffuse.xyz, surface_diffuse.w);
}
