/* This file implements standard programs for depth shadow mapping. 
   These particular ones are suitable for additive lighting models, and
   include 3 techniques to reduce depth fighting on self-shadowed surfaces,
   constant bias, gradient (slope-scale) bias, and a fuzzy shadow map comparison*/

// Shadow caster vertex program.
void casterVP(
	float4 position			: POSITION,
	float4 texcoord			: TEXCOORD0,
	out float4 outPos		: POSITION,
	out float2 outDepth		: TEXCOORD0,
	out float2 outTex		: TEXCOORD1,


	uniform float4x4 worldViewProj,
	uniform float4 texelOffsets,
	uniform float4 depthRange
	)
{
	outPos = mul(worldViewProj, position);
	outTex = texcoord.xy;
	// fix pixel / texel alignment
	outPos.xy += texelOffsets.zw * outPos.w;
	// linear depth storage
	// offset / scale range output
#ifdef LINEAR_RANGE
	outDepth.x = (outPos.z - depthRange.x) * depthRange.w;
#else
	outDepth.x = outPos.z;
#endif
	outDepth.y = outPos.w;
}


// Shadow caster fragment program for high-precision single-channel textures	
void casterFP(
	float2 depth			: TEXCOORD0,
	float2 texcoord : TEXCOORD1,
   	uniform sampler2D tex,
	out float4 result		: COLOR)
	
{
#ifdef LINEAR_RANGE
	float finalDepth = depth.x;
#else
	float finalDepth = depth.x / depth.y;
#endif
	float4 trans = tex2D(tex,texcoord);
	
	
	//finalDepth = finalDepth*0.001 +finalDepth;
	//no alpha info?
	if(trans.x == 0.0) 
	{
		trans.w = 1;
	}
	clip(trans.w-0.5);

	// just smear across all components 
	// therefore this one needs high individual channel precision
	result = float4(finalDepth, finalDepth, finalDepth, 1);
}
