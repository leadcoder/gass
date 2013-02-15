
#include "common.shader"

#ifdef BASE_MAP
	#ifdef NORMAL_MAP
		#define BASE_MAP_REG s0
		#define NORMAL_MAP_REG s1
		#define SHADOW_MAP_REG s2
		#define DETAIL1_REG s3
		#define DETAIL2_REG s4
		#define DETAIL3_REG s5
		#define DETAIL4_REG s6
	#else
		#define BASE_MAP_REG s0
		#define SHADOW_MAP_REG s1
		#define DETAIL1_REG s2
		#define DETAIL2_REG s3
		#define DETAIL3_REG s4
		#define DETAIL4_REG s5
		
	#endif
#else
	#ifdef NORMAL_MAP
		#define NORMAL_MAP_REG s0
		#define SHADOW_MAP_REG s1
		#define DETAIL1_REG s2
		#define DETAIL2_REG s3
		#define DETAIL3_REG s4
		#define DETAIL4_REG s5
	#else
		#define SHADOW_MAP_REG s0
		#define DETAIL1_REG s1
		#define DETAIL2_REG s2
		#define DETAIL3_REG s3
		#define DETAIL4_REG s4
	#endif
#endif


void PerPixelVert(float4 position : POSITION 
                    ,float3 normal   : NORMAL
                    ,uniform float4x4 worldviewproj
					,uniform float4x4 worldMat
					,uniform float4   lightPosition					
                    ,out float4 oPosition   : POSITION
                    ,out float3 oObjectPos   : TEXCOORD1
                    ,out float3 oNormal   : TEXCOORD2
					,out float OLightDist : TEXCOORD5
#ifdef NORMAL_MAP
					,float2 uv         : TEXCOORD0
					,out float2 oUv    : TEXCOORD0
//#ifndef GENERATE_TANGENT
					,out float3 oTangent : TEXCOORD3
				    ,float3 tangent     : TANGENT0
//#endif
#elif BASE_MAP
					,float2 uv         : TEXCOORD0
					,out float2 oUv    : TEXCOORD0
#endif
#ifdef INTEGRATED_SHADOWS
					,out float4 oShadowUV : TEXCOORD4
					,uniform float4x4 texWorldViewProj
#ifdef LINEAR_RANGE
					,uniform float4 shadowDepthRange
#endif
#endif
					) 
{ 
   oPosition = mul(worldviewproj, position); 
   oObjectPos = position.xyz;
   oNormal = normal;
   
   float3 lightDir = lightPosition.xyz -  (position.xyz * lightPosition.w);
   
   worldMat[0][3] = 0;
   worldMat[1][3] = 0;
   worldMat[2][3] = 0;
   lightDir	= mul(worldMat,float4(lightDir,1));
   OLightDist = length(lightDir);
#ifdef NORMAL_MAP
	oUv = uv; 
	oTangent = tangent;
#elif BASE_MAP
	oUv = uv;
#endif
#ifdef INTEGRATED_SHADOWS
    oShadowUV = mul(texWorldViewProj, position);
#ifdef LINEAR_RANGE
	oShadowUV.z = (oShadowUV.z - shadowDepthRange.x) * shadowDepthRange.w;
#endif
#endif 
} 

void PerPixelFrag(float4 position   : TEXCOORD1,
						  float3 normal      : TEXCOORD2, 
						  uniform float4   lightPosition,
                          uniform float3   eyePosition, 
                          uniform float4   lightDiffuse,
                          uniform float4   lightSpecular,
                          uniform float    exponent,
                          //uniform float4   ambient,
						  uniform float4 lightAtt,
                          uniform float4 spotParams,
						  uniform float3 spotDirection,
						  float lightDist      : TEXCOORD5,
#ifdef BASE_MAP
						  uniform sampler2D baseMap : register(BASE_MAP_REG),
#endif 
#ifdef NORMAL_MAP
						  float2 uv  : TEXCOORD0,
						  uniform sampler2D normalHeightMap : register(NORMAL_MAP_REG),
						  float3 tangent      : TEXCOORD3,
#ifdef PARALLAX_EFFECT					
						  uniform float2 displacementScaleBias,
#endif						  
#elif BASE_MAP
						  float2 uv  : TEXCOORD0,
#endif

#ifdef INTEGRATED_SHADOWS
						  float4 shadowUV : TEXCOORD4,
						  uniform sampler2D shadowMap : register(SHADOW_MAP_REG),
#endif
#if DETAIL_SPLATTING
						  uniform float4 splatScales,
						  uniform sampler2D coverageMap: register(DETAIL1_REG),
						  uniform sampler2D splat1Map: register(DETAIL2_REG),
	                      uniform sampler2D splat2Map: register(DETAIL3_REG),
						  uniform sampler2D splat3Map: register(DETAIL4_REG),
#endif 
                          out float4 oColor : COLOR) 
{
	float3 N = normalize(normal);
	float3 eyeDir = eyePosition - position.xyz;
#ifdef STD_FOG
	float eyeDistance = length(eyeDir);
#endif
	eyeDir = normalize(eyeDir);
	float3 lightDir = lightPosition.xyz -  (position.xyz * lightPosition.w);
	lightDir = normalize(lightDir);
	if(lightDist > lightAtt.x)
		clip(-1.0);

#ifdef GENERATE_TANGENT
	tangent = float3(-1,0,0);
	N = expand(tex2D(normalHeightMap, uv).xyz);
	float3 bin = normalize(cross(tangent ,N)); 
	tangent = normalize(cross(N, bin));
#endif		

#ifdef NORMAL_MAP
	float3 T = normalize(tangent);
	float3 binormal = normalize(cross(T ,N)); 
	float3x3 rotation = float3x3(T, binormal, N); 
    eyeDir = mul(rotation, eyeDir);
	lightDir = mul(rotation, lightDir);
	spotDirection = mul(rotation, spotDirection);
#ifdef PARALLAX_EFFECT
	// get the height using the tex coords
	float height = tex2D(normalHeightMap, uv).a;
	//scale and bias factors	
	float scale = displacementScaleBias.x;
	float bias = displacementScaleBias.y;
	// calculate displacement	
	float displacement = (height * scale) + bias;
	float3 uv2 = float3(uv, 1);
	float3 scaledEyeDir = eyeDir * displacement;
	// calculate the new tex coord to use for normal and diffuse
	float2 texCoord = (scaledEyeDir + uv2).xy;
#else
	float2 texCoord = uv.xy;
#endif
	// get the new normal and diffuse values
	N = expand(tex2D(normalHeightMap, texCoord).xyz);
#elif BASE_MAP
	float2 texCoord = uv;
#endif
#ifdef BASE_MAP
	float4 base_map = tex2D(baseMap, texCoord);
	lightDiffuse.xyz = lightDiffuse.xyz * base_map.xyz;
	float alpha	 = base_map.a;
#else
	float alpha	 = 1.0;
#endif

#if DETAIL_SPLATTING
	lightDiffuse.xyz = calcSplatting(coverageMap,splat1Map,splat2Map,splat3Map,texCoord,splatScales,lightDiffuse.xyz);
#endif
	
   float3 halfAngle =  normalize(eyeDir + lightDir);
   float4 lightColor =  calcSpotlight(N, lightDir,halfAngle, lightDiffuse,lightSpecular,  exponent, spotParams, spotDirection,lightAtt,lightDist);

#ifdef INTEGRATED_SHADOWS
	float visibility = calcShadow(shadowUV,shadowMap);
	lightColor = lightColor*visibility;
#endif
#ifdef STD_FOG
	lightColor.xyz = calcFog(eyeDistance,fogParams, fogColor, lightColor.xyz);
#endif
	oColor = float4(lightColor.xyz, alpha*lightDiffuse.a);
}  
