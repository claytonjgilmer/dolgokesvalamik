//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------

float4x4 worldViewProj : WorldViewProjection; // This matrix will be loaded by the application
float4x4 WorldMtx;
float3 light_dir;

//texture testTexture; // This texture will be loaded by the application

sampler Sampler = sampler_state
{
//    Texture   = (testTexture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//-----------------------------------------------------------------------------
// Vertex Definitions
//-----------------------------------------------------------------------------

// Our sample application will send vertices 
// down the pipeline laid-out like this...

struct VS_INPUT
{
    float3 position	: POSITION;
    float3 normal	: NORMAL;
	float2 texture0 : TEXCOORD0;
};

// Once the vertex shader is finished, it will 
// pass the vertices on to the pixel shader like this...

struct VS_OUTPUT
{
    float4 hposition : POSITION;
    float3 hnormal	 : TEXCOORD1;
	float2 texture0  : TEXCOORD0;
    float4 color	 : COLOR0;
};

// And finally, the pixel shader will send a single 
// color value to the frame buffer like this...

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Simple Vertex Shader
//-----------------------------------------------------------------------------

VS_OUTPUT myvs( VS_INPUT IN )
{
    VS_OUTPUT OUT;

	OUT.hposition = mul( worldViewProj, float4(IN.position, 1) );

	OUT.color = float4( .5, .5, .5, .5 ); // Pass white as a default color
	OUT.hnormal=mul (WorldMtx,float4(IN.normal,0));
//	OUT.hnormal=IN.normal;

	OUT.texture0 = IN.texture0;

	return OUT;
}

//-----------------------------------------------------------------------------
// Simple Pixel Shader
//-----------------------------------------------------------------------------

PS_OUTPUT myps( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;
    
    float3 normal=normalize(IN.hnormal);

	OUT.color = tex2D( Sampler, IN.texture0 ) * saturate(max(pow((dot(normal,light_dir)+1)/2,2),.3));

	// If you uncomment the next line, the color passed to us by our
    // vertex shader and the selected texture sampler will be ignored 
	// completely and all output to the frame buffer will be blue regardless.

	//OUT.color = float4( 0.0, 0.0, 1.0, 1.0);

    return OUT;
}

//-----------------------------------------------------------------------------
// Simple Effect (1 technique with 1 pass)
//-----------------------------------------------------------------------------

technique Technique0
{
    pass Pass0
    {
		Lighting = FALSE;

		Sampler[0] = (Sampler); // Needed by pixel shader

		VertexShader = compile vs_2_0 myvs();
		PixelShader  = compile ps_2_0 myps();
    }
}

