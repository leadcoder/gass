#ifndef DISABLE_VC
	//#define VERTEX_COLOR 
#endif

float3 expand(float3 v)
{
	return (v - 0.5) * 2;
}
	
float calcShadow(float4 shadowUV,sampler2D shadowMap)
{
   // Generate the 9 texture co-ordinates for a 3x3 PCF kernel
   float4 vTexCoords[9];
   // Texel size
   float fTexelSize = 1.0f /2048.0f;

   shadowUV = shadowUV / shadowUV.w;
   float center_depth = tex2D( shadowMap, shadowUV.xy ).x;
   if(center_depth >= 1.0)
	 return 1.0;

   // Generate the tecture co-ordinates for the specified depth-map size
   // 4 3 5
   // 1 0 2
   // 7 6 8
   //vTexCoords[0] = shadowUV;
   vTexCoords[1] = shadowUV+ float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
   vTexCoords[2] = shadowUV + float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
   vTexCoords[3] = shadowUV + float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[6] = shadowUV + float4( 0.0f,  fTexelSize, 0.0f, 0.0f );
   vTexCoords[4] = shadowUV + float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[5] = shadowUV + float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
   vTexCoords[7] = shadowUV + float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
   vTexCoords[8] = shadowUV + float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );
   // Sample each of them checking whether the pixel under test is shadowed or not
   //float fShadowTerms[9];
   float fShadowTerm = 0.0f;
   float fixedDepthBias = 0.0000001;
   float compareDepth = shadowUV.z - fixedDepthBias;
   for( int i = 1; i < 9; i++ )
   {
      float A = tex2D( shadowMap, vTexCoords[i].xy ).x;
      
      // Texel is shadowed
      //fShadowTerms[i] = A < compareDepth ? 0.0f : 1.0f;
	  fShadowTerm += A < compareDepth ? 0.0f : 1.0f;
      //fShadowTerm   += fShadowTerms[i];
   }
   fShadowTerm += center_depth < compareDepth ? 0.0f : 1.0f;
   
   // Get the average
   fShadowTerm /= 9.0f;
   return fShadowTerm;
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
	float NdotL = max(dot(lightDir, normal), 0);
	float NdotH = max(dot(halfAngle, normal), 0);
    float4 Lit = saturate(lit(NdotL,NdotH,exponent));
	float3 col = diffuse.xyz*lightDiffuse * Lit.y + lightSpecular * Lit.z;
	return col;
}

float4 calcSpotlight(float3 normal,
	float3 lightDir,
	float3 halfAngle,
	float4 lightDiffuse,
	float4 lightSpecular, 
	float exponent, 
	float4 spotParams,
	float3 spotDir,
	float4 lightAtten,
	float lightDist)
{
   float NdotL = max(dot(lightDir, normal), 0);
   float NdotH = max(dot(halfAngle, normal), 0);
   float4 litRes = lit(NdotL, NdotH, exponent);
   float rho = saturate(dot(-spotDir, lightDir));
   // factor = (rho - cos(outer/2) / cos(inner/2) - cos(outer/2)) ^ falloff
   float spotFactor = pow(saturate(rho - spotParams.y) / (spotParams.x - spotParams.y), spotParams.z);
   float4 lightColor = spotFactor*lightDiffuse * litRes.y + spotFactor*lightSpecular * litRes.z;
   float4 attn = ( 1.0 / (( lightAtten.y ) + ( lightAtten.z * lightDist ) + ( lightAtten.w * lightDist * lightDist )));
   return lightColor*attn;
}

float3 calcSplatting(sampler2D coverageMap, sampler2D splat1Map, sampler2D splat2Map, sampler2D splat3Map,float2 texCoord,float4 splatScales,float3 lightDiffuse)
{
	float3 coverage = tex2D(coverageMap, texCoord);
	float3 inv_coverage = (1 - coverage.xyz)*0.5;
    float3 detail1  =   tex2D(splat1Map, texCoord* splatScales.x).xyz;
    detail1 = detail1*coverage.x + inv_coverage.x;
    lightDiffuse  = lightDiffuse*detail1;
   	float3 detail2  =  tex2D(splat2Map, texCoord* splatScales.y).xyz;
   	detail2 = detail2*coverage.y + inv_coverage.y;
   	lightDiffuse  = lightDiffuse*detail2*2;
	float3 detail3  =   tex2D(splat3Map, texCoord* splatScales.z).xyz;
	detail3 = detail3*coverage.z + inv_coverage.z;
	lightDiffuse  = lightDiffuse*detail3*2;
	return lightDiffuse;
}


float3 calcFadeSplatting(sampler2D coverageMap, 
sampler2D splat1Map, 
sampler2D splat2Map, 
sampler2D splat3Map,
float2 texCoord,
float4 splatScales,
float3 global_color, 
float near_color_weight, 
float detail_fade_dist,
float eye_dist)
{
	float3 coverage = tex2D(coverageMap, texCoord);
	float3 inv_coverage = (1 - coverage.xyz)*0.5;
    float3 detail1  =   tex2D(splat1Map, texCoord* splatScales.x).xyz;
    detail1 = detail1*coverage.x;// + inv_coverage.x;
	//float3 lightDiffuse  = global_color*detail1;
    float3 detail2  =  tex2D(splat2Map, texCoord* splatScales.y).xyz;
   	detail2 = detail2*coverage.y;// + inv_coverage.y;
	//lightDiffuse  = lightDiffuse*detail2*2;
   	float3 detail3  =   tex2D(splat3Map, texCoord* splatScales.z).xyz;
	detail3 = detail3*coverage.z;// + inv_coverage.z;
	//lightDiffuse  = lightDiffuse*detail3*2;
	
	float fade_val = saturate((eye_dist - detail_fade_dist) /  detail_fade_dist);
	//float3 final_color  = lerp(detail1 + detail2 + detail3, global_color, fade_val);
	float3 final_color  = lerp(lerp((detail1 + detail2 + detail3)*global_color,global_color*0.5, near_color_weight), global_color*0.5, fade_val);
	return final_color;
}

			
