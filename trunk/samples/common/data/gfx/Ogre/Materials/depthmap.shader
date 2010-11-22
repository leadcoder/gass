
void depthmap_vs(
    in float4 p : POSITION, in float3 n : NORMAL,
    out float4 cp : POSITION,
    out float4 vp : TEXCOORD0, // view-space position
    out float4 vn : TEXCOORD1,
    uniform float4x4 wvpMat, uniform float4x4 wvMat)
{
    cp = mul(wvpMat, p);
    vp = mul(wvMat, p);
    vn = mul(wvMat, float4(n, 0));
}

struct geomOut
{
    float4 c : COLOR0;
};

geomOut depthmap_ps(in float4 vp : TEXCOORD0, in float4 vn : TEXCOORD1, uniform float far)
{
     geomOut OUT;
    float COEF = 0.5; // <-- thats the value we computed in step 3
     OUT.c = float4(length(vp.xyz) * COEF / far, normalize(vn.xyz).xyz);
    return OUT;

}

void depthmap_max_vs(
    in float4 p : POSITION,
    out float4 cp : POSITION,
	uniform float4x4 wvpMat)
{
    cp = mul(wvpMat, p);
}


geomOut depthmap_max_ps(uniform float far)
{
     geomOut OUT;
     OUT.c = far;
     return OUT;
}
