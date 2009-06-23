// Shadow mapping pixel shader
/*float calcShadow(float4 shadowUV,sampler2D shadowMap)
{
   // Generate the 9 texture co-ordinates for a 3x3 PCF kernel
   float4 vTexCoords[21];
   // Texel size
   float fTexelSize = 1.0f /1024.0f;

   float fTexelSize2 = fTexelSize*2;

   shadowUV = shadowUV / shadowUV.w;

   // Generate the tecture co-ordinates for the specified depth-map size
   // 4 3 5
   // 1 0 2
   // 7 6 8
   vTexCoords[0] = shadowUV;
   vTexCoords[1] = shadowUV+ float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   vTexCoords[2] = shadowUV + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   vTexCoords[3] = shadowUV + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[6] = shadowUV + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   vTexCoords[4] = shadowUV + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[5] = shadowUV + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[7] = shadowUV + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   vTexCoords[8] = shadowUV + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );

   vTexCoords[9] = shadowUV+ float4( -fTexelSize2, 0.0f, 0.0f, 0.0f );
   vTexCoords[10] = shadowUV + float4( -fTexelSize2, fTexelSize, 0.0f, 0.0f );
   vTexCoords[11] = shadowUV + float4( -fTexelSize2, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[12] = shadowUV + float4( 	fTexelSize2, 0.0f, 0.0f, 0.0f );
   vTexCoords[13] = shadowUV + float4(  fTexelSize2, fTexelSize, 0.0f, 0.0f );
   vTexCoords[14] = shadowUV + float4(  fTexelSize2, -fTexelSize, 0.0f, 0.0f );

   vTexCoords[15] = shadowUV+   float4(0.0f, -fTexelSize2,  0.0f, 0.0f );
   vTexCoords[16] = shadowUV + float4( -fTexelSize,-fTexelSize2, 0.0f, 0.0f );
   vTexCoords[17] = shadowUV + float4(fTexelSize, -fTexelSize2,  0.0f, 0.0f );
   vTexCoords[18] = shadowUV+   float4(0.0f, fTexelSize2,  0.0f, 0.0f );
   vTexCoords[19] = shadowUV + float4( -fTexelSize,fTexelSize2, 0.0f, 0.0f );
   vTexCoords[20] = shadowUV + float4(fTexelSize, fTexelSize2,  0.0f, 0.0f );
      
   
   // Sample each of them checking whether the pixel under test is shadowed or not
   float fShadowTerms[21];
   float fShadowTerm = 0.0f;
   float fixedDepthBias = 0.0000001;
   float compareDepth = shadowUV.z - fixedDepthBias;
   for( int i = 0; i < 21; i++ )
   {
      float A = tex2D( shadowMap, vTexCoords[i].xy ).x;
      float B = compareDepth;

      // Texel is shadowed
      fShadowTerms[i] = A < B ? 0.0f : 1.0f;
      fShadowTerm     += fShadowTerms[i];
   }
   // Get the average
   fShadowTerm /= 21.0f;
   return fShadowTerm;
}*/


float calcShadow(float4 shadowUV,sampler2D shadowMap)
{
   // Generate the 9 texture co-ordinates for a 3x3 PCF kernel
   float4 vTexCoords[9];
   // Texel size
   float fTexelSize = 1.0f /2048.0f;

   shadowUV = shadowUV / shadowUV.w;

   // Generate the tecture co-ordinates for the specified depth-map size
   // 4 3 5
   // 1 0 2
   // 7 6 8
   vTexCoords[0] = shadowUV;
   vTexCoords[1] = shadowUV+ float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   vTexCoords[2] = shadowUV + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   vTexCoords[3] = shadowUV + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[6] = shadowUV + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   vTexCoords[4] = shadowUV + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[5] = shadowUV + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[7] = shadowUV + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   vTexCoords[8] = shadowUV + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );
   // Sample each of them checking whether the pixel under test is shadowed or not
   float fShadowTerms[9];
   float fShadowTerm = 0.0f;
   float fixedDepthBias = 0.0000001;
   float compareDepth = shadowUV.z - fixedDepthBias;
   for( int i = 0; i < 9; i++ )
   {
      float A = tex2D( shadowMap, vTexCoords[i].xy ).x;
      float B = compareDepth;

      // Texel is shadowed
      fShadowTerms[i] = A < B ? 0.0f : 1.0f;
      fShadowTerm     += fShadowTerms[i];
   }
   // Get the average
   fShadowTerm /= 9.0f;
   return fShadowTerm;
}
/*
float calcShadow(float4 shadowUV,sampler2D shadowMap)
{
	float gradientClamp = 0.0098;
	float gradientScaleBias = 0;
	float fixedDepthBias = 0.0000001;
	float inverseShadowmapSize = 1.0/2048.0;//0.00048828125;

	shadowUV = shadowUV / shadowUV.w;
	
        //gradient calculation
  	half pixelOffset = inverseShadowmapSize;
  	
  	float4 values = float4(
			tex2D( shadowMap, shadowUV.xy ).x,
			tex2D( shadowMap, shadowUV.xy + float2(pixelOffset, 0) ).x,
			tex2D( shadowMap, shadowUV.xy + float2(0, pixelOffset) ).x,
			tex2D( shadowMap, shadowUV.xy + float2(pixelOffset, pixelOffset) ).x);
			

        float compareDepth = shadowUV.z - fixedDepthBias;

	//transform to texel space
	float2 texelPos = 1.0/inverseShadowmapSize * shadowUV.xy;
	
	//determine the lerp amounts           
	float2 lerps = frac( texelPos );
	//float2 lerps = 	texelPos;	
	//lerps.x = saturate(lerps.x); 
	//lerps.y = saturate(lerps.y);

	values.x = values.x < compareDepth ? 1.0f : 0.0f;
	values.y = values.y < compareDepth ? 1.0f : 0.0f;
	values.z = values.z < compareDepth ? 1.0f : 0.0f;
	values.w = values.w < compareDepth ? 1.0f : 0.0f;
		
	//lerp between the shadow values to calculate our light amount
	float final = lerp( 
			lerp( values.x, values.y, lerps.x ),
			lerp( values.z, values.w, lerps.x ),
			lerps.y );
	return 1.0-final;
}*/




float calcVarianceShadow(float4 shadowUV,sampler2D shadowMap)
{
	float4 scoord = shadowUV / shadowUV.w;
    // grab the samples
    float2 datum  = tex2D(shadowMap , scoord.xy).xy;
     	//-- Variance Shadow Mapping ---------------------------------------------------

     	float zVariance = datum.y - (datum.x * datum.x);
     	float zDeviation = scoord.z - datum.x;
     	zDeviation = (zDeviation < 0.0) ? 0.0 : zDeviation;
     	float visibility = zVariance / (zVariance + (zDeviation * zDeviation));
     	float ztest = (scoord.z < datum.x) ? 1.0:0.0;  // filtering depth ok, because used only for small variance
     	visibility = (zVariance > 0.0) ? visibility : ztest; // if variance too small, we get garbage
	return visibility;
}



float3 calcFog(float eyeDistance, float4 fogParams, float3 fogColor, float3 color)
{
	float fog = 0.0;

//#ifdef FOG_LINEAR
if(fogParams.z > 1) //we have linear fog?
	fog = ( fogParams.z - eyeDistance ) * fogParams.w; // / ( fogParams.z - fogParams.y );
//#endif
else
{
#ifdef FOG_EXP
	float fogDensity = fogParams.x;
	//fog = 1.0f / exp( ( eyeDistance * fogDensity ) * log( 2.718281828f ) );
	fog = exp( -abs( eyeDistance * fogDensity ) );
#endif

#ifdef FOG_EXP2
	float fogDensity = fogParams.x;
	float m = eyeDistance * fogDensity;
	//fog = 1.0f / exp( ( m * m ) * log( 2.718281828f ) );
	fog = exp( -(m*m ) );
#endif
}
	fog = saturate(fog);
	float3 out_color = float3(lerp(fogColor.xyz, color, fog));
	return out_color;
}


float3 calcLight(float4 diffuse,float3 normal,float3 lightDir,float3 halfAngle,float3 lightDiffuse,float3 lightSpecular, float exponent)
{
	float3 N = normalize(normal);
	float NdotL = dot(normalize(lightDir), N);
	float NdotH = dot(normalize(halfAngle), N);
	float4 Lit = saturate(lit(NdotL,NdotH,exponent));
	float3 col = diffuse.xyz*lightDiffuse * Lit.y + lightSpecular * Lit.z;
	return col;
}

void diffuse_one_light_vp(float4 position   : POSITION, 
	float3 normal      : NORMAL, 
        float2 uv         : TEXCOORD0, 
        // outputs 
        out float4 oPosition    : POSITION, 
        out float2 oUv          : TEXCOORD0,
        out float4 oEyeDir       : TEXCOORD1,
#ifndef LIGHT_MAP 
        out float3 oLightDir    : TEXCOORD2, 
		out float3 oSpotDirection : TEXCOORD3, 
		out float3 oNormal :  TEXCOORD4, 
		out float3 oHalfAngle:  TEXCOORD5,
#endif
#ifdef INTEGRATED_SHADOWS
        out float4 oShadowUV : TEXCOORD6,
#endif
		// parameters 
		uniform float4 lightPosition, // object space 
        uniform float3 eyePosition,   // object space 
        uniform float3 spotDirection, // object space
        uniform float4x4 worldViewProj
#ifdef INTEGRATED_SHADOWS
#ifdef LINEAR_RANGE
		  ,uniform float4 shadowDepthRange
#endif
		  ,uniform float4x4 worldMatrix
		  ,uniform float4x4 texViewProj 
#endif
		)
{  
   //calculate output position 
   oPosition = mul(worldViewProj, position); 
  
   // pass the main uvs straight through unchanged 
   oUv = uv;
   float3 eyeDir = eyePosition.xyz - position.xyz; 
   oEyeDir.xyz = normalize(eyeDir);
   oEyeDir.w =  length(eyeDir);
#ifndef LIGHT_MAP
   float3 lightDir = normalize(lightPosition.xyz -  (position.xyz * lightPosition.w).xyz);
   oNormal = normal;
   oLightDir = normalize(lightDir); 
   oHalfAngle = normalize(eyeDir + lightDir); 
   oSpotDirection = normalize(-spotDirection);
#endif
#ifdef INTEGRATED_SHADOWS
   float4 worldPos = mul(worldMatrix, position);
   oShadowUV = mul(texViewProj, worldPos);
#ifdef LINEAR_RANGE
	oShadowUV.z = (oShadowUV.z - shadowDepthRange.x) * shadowDepthRange.w;
#endif
#endif 
}

void diffuse_one_light_fp(
	float2 uv : TEXCOORD0,
	float4 eyeDir : TEXCOORD1,
	float3 lightDir : TEXCOORD2,
	float3 spotDir : TEXCOORD3,
	float3 normal : TEXCOORD4,
	float3 halfAngle : TEXCOORD5,
#ifdef INTEGRATED_SHADOWS
	float4 shadowUV : TEXCOORD6,
#endif
	uniform float4 shadowParams,	
	uniform float3 lightSpecular,
	uniform float exponent,
	uniform float3 lightDiffuse,
	uniform float4 ambient,
#ifdef STD_FOG
	uniform float3 fogColor,
    uniform float4 fogParams, 
#endif
	uniform sampler2D diffuseMap : register(s0),
#ifdef INTEGRATED_SHADOWS
	uniform sampler2D shadowMap : register(s1),
#endif
	out float4 oColour : COLOR)
{
	float4 diffuse = tex2D(diffuseMap, uv);
	float3 color = calcLight(diffuse,normal,lightDir,halfAngle,lightDiffuse,lightSpecular,exponent);
#ifdef INTEGRATED_SHADOWS
	float visibility = calcShadow(shadowUV,shadowMap);
	color = color*visibility;
#endif
color = color + ambient.xyz*diffuse.xyz;
#ifdef STD_FOG
	float eyeDistance = eyeDir.w;
	color = calcFog(eyeDistance,fogParams, fogColor, color);
#endif
	oColour = float4(color, diffuse.a);
}



#ifdef LIGHT_MAP
	#if NUM_SPLATS  == 0
		#define LIGHT_MAP_REG s1
		#define SHADOW_MAP_REG s2
	#endif
	#if NUM_SPLATS == 1
		#define LIGHT_MAP_REG s3
		#define SHADOW_MAP_REG s4
	#endif
	#if NUM_SPLATS == 2
		#define LIGHT_MAP_REG s4
		#define SHADOW_MAP_REG s5
	#endif
	#if NUM_SPLATS == 3
		#define LIGHT_MAP_REG s5
		#define SHADOW_MAP_REG s6
	#endif
#else
	#if NUM_SPLATS  == 0
		#define SHADOW_MAP_REG s1
	#endif
	#if NUM_SPLATS == 1
		#define SHADOW_MAP_REG s3
	#endif
	#if NUM_SPLATS == 2
		#define SHADOW_MAP_REG s4
	#endif
	#if NUM_SPLATS == 3
		#define SHADOW_MAP_REG s5
	#endif
#endif



void terrain_fp(
	float2 uv : TEXCOORD0,
	float4 eyeDir : TEXCOORD1,
#ifndef LIGHT_MAP
	float3 lightDir : TEXCOORD2,
	float3 spotDir : TEXCOORD3,
	float3 normal : TEXCOORD4,
	float3 halfAngle : TEXCOORD5,
	uniform float3 lightDiffuse,
	uniform float3 lightSpecular,
	uniform float exponent,
#endif
#ifdef INTEGRATED_SHADOWS
	float4 shadowUV : TEXCOORD6,
#endif
#if NUM_SPLATS > 0
	uniform float4 splatScales,
#endif
	uniform float4 ambient,
#ifdef STD_FOG
	uniform float3 fogColor,
    uniform float4 fogParams, 
#endif
	uniform sampler2D baseMap: register(s0),
#if NUM_SPLATS >= 1
	uniform sampler2D coverageMap: register(s1),
	uniform sampler2D splat1Map: register(s2),
#endif
#if NUM_SPLATS >= 2
	uniform sampler2D splat2Map: register(s3),
#endif
#if NUM_SPLATS >= 3
	uniform sampler2D splat3Map: register(s4),
#endif
#ifdef LIGHT_MAP
	uniform sampler2D lightMap: register(LIGHT_MAP_REG),
#endif
#ifdef INTEGRATED_SHADOWS
	uniform sampler2D shadowMap : register(SHADOW_MAP_REG),
#endif
	out float4 oColour : COLOR)
{

	float4 diffuse  = tex2D(baseMap, uv);
#if NUM_SPLATS >= 1
	float4 coverage = tex2D(coverageMap, uv);
	float3 inv_coverage = (1 - coverage.xyz)*0.5;
    	float4 detail1  =   tex2D(splat1Map, uv* splatScales.x);
    	detail1 = detail1*coverage.x + inv_coverage.x;
    	diffuse  = diffuse*detail1;
#endif
#if NUM_SPLATS >= 2
    	float4 detail2  =   tex2D(splat2Map, uv* splatScales.y);
    	detail2 = detail2*coverage.y + inv_coverage.y;
    	diffuse  = diffuse*detail2*2;
#endif
#if NUM_SPLATS >= 3
	float4 detail3  =   tex2D(splat3Map, uv* splatScales.z);
	detail3 = detail3*coverage.z + inv_coverage.z;
	diffuse  = diffuse*detail3*2;
#endif

#ifdef LIGHT_MAP
	float3 color = tex2D(lightMap, uv).xyz * diffuse.xyz;
#else
	float3 color = calcLight(diffuse,normal,lightDir,halfAngle,lightDiffuse,lightSpecular,exponent);
#endif
#ifdef INTEGRATED_SHADOWS
	float visibility = calcShadow(shadowUV,shadowMap);
	color = color*visibility;
#endif
	color = color + ambient.xyz*diffuse.xyz;
#ifdef STD_FOG
	float eyeDistance = eyeDir.w;
	color = calcFog(eyeDistance,fogParams, fogColor, color);
#endif
	oColour = float4(color, diffuse.a);
}



void vegetation_fp(
	float2 uv : TEXCOORD0,
#ifdef INTEGRATED_SHADOWS
	float4 shadowUV : TEXCOORD1,
#endif
	float4 color : COLOR,
#ifdef INTEGRATED_SHADOWS
	uniform float4 shadowParams,
#endif	
	uniform float4 ambient,
	uniform float4 lightDiffuse,
	uniform float3 lightDir,
	
	uniform sampler2D diffuseMap : register(s0),
#ifdef INTEGRATED_SHADOWS
	uniform sampler2D shadowMap : register(s1),
#endif
	out float4 oColour : COLOR)
{
	float4 diffuse = tex2D(diffuseMap, uv);
	
	lightDir = normalize(lightDir);
	float3 lit_diffuse = diffuse.xyz*lightDir.y;

#ifdef INTEGRATED_SHADOWS
	float visibility = calcShadow(shadowUV,shadowMap);
	lit_diffuse = lit_diffuse*visibility;
#endif
	oColour = float4(lit_diffuse*color.xyz*lightDiffuse.xyz + ambient.xyz*diffuse.xyz*color.xyz, diffuse.w*color.w);
}
