cbuffer cbChangesPerFrame : register(b0)
{
	float4 fadeColor_;
	float4 backColorUp_;
	float4 backColorDown_;
};

Texture2D texture0_ : register(t0);
SamplerState texture0Sampler_: register(s0);
Texture2D texture1_ : register(t1);
SamplerState texture1Sampler_: register(s1);
Texture2D texture2_ : register(t2);
SamplerState texture2Sampler_: register(s2);
Texture2D texture3_ : register(t3);
SamplerState texture3Sampler_: register(s3);
Texture2D textureChar_ : register(t4);
SamplerState textureCharSampler_: register(s4);



struct VS_Input
{
	float4 pos : POSITION;
	float4 tex0 : TEXCOORD0;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 fadeColor : COLOR0;
	float4 backColorUp : COLOR1;
	float4 backColorDown : COLOR2;

};


PS_Input vs_main(VS_Input vertex)
{
	
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0.xy;
	vsOut.fadeColor = fadeColor_;
	vsOut.backColorUp = backColorUp_;
	vsOut.backColorDown = backColorDown_;

	//vsOut.backColorUp = float4(-.1, .6, .7, 1.0);
	//vsOut.backColorDown = float4(-.1, .6, .7, 1.0);
	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	float fade = .25 - .25 * frag.fadeColor.x;
	float samp0Fade = fade;
	float samp1Fade = fade + .25;
	float samp2Fade = fade + .5;
	float samp3Fade = fade + .75;

	samp0Fade *= samp0Fade;
	samp1Fade *= samp1Fade;
	samp2Fade *= samp2Fade;
	samp3Fade *= samp3Fade;

	float2 sampCoord = frag.tex0.xy;
	float edgeVal = ( frag.tex0.x - .5 ) * 2.0;
	edgeVal *= edgeVal * -.2;
	sampCoord.y = pow(sampCoord.y, 1.2 + edgeVal);

	float samp3Alpha = min(10.0 - 10.0 * samp3Fade, 1.0);
	float samp0 = texture0_.Sample(texture0Sampler_, sampCoord).x;
	float samp1 = texture1_.Sample(texture1Sampler_, sampCoord).x;
	float samp2 = texture2_.Sample(texture2Sampler_, sampCoord).x;
	float samp3 = texture3_.Sample(texture3Sampler_, sampCoord).x * samp3Alpha;

	float sampChar = textureChar_.Sample(textureCharSampler_, sampCoord).x;

	
	float col = samp0Fade * samp0;
	col += samp1 * (samp1Fade - col);
	col += sampChar * (.25 - col);
	col += samp2 * (samp2Fade - col);
	col += samp3 * (samp3Fade - col);
	col = 1.0 - col;
	col *= col;
	
	float h = frag.tex0.y;
	//float3 backColor0 = float3(-.1, .6, .7);// backColorUp_
	//float3 backColor1 = float3(.7, .35, -.1);// backColorDown_
	//float3 backColor = backColor0 + h * ( backColor1 - backColor0 );
	float3 backColor = frag.backColorUp.xyz + h * (frag.backColorDown.xyz - frag.backColorUp.xyz);
	float3 color = col * backColor;

	float2 uvCenterDiff = frag.tex0.xy - float2(.5, .5);
	float vignette = dot( uvCenterDiff, uvCenterDiff);
	vignette *= vignette;
	vignette = 1.0 - 2.0 * vignette;
	
	
	color += backColor * .3;
	color *= vignette;
	color -= color * frag.fadeColor.w;
	return float4(color, 1.0);
}