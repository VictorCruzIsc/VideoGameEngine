
struct VERTEX_INPUT
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

struct VERTEX_OUTPUT
{
    float4 Position : SV_Position;
    float4 PositionUnprojected : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};
VERTEX_OUTPUT VSMain(VERTEX_INPUT Input)
{
    VERTEX_OUTPUT Output;
    Output.Position = Input.Position;
    Output.PositionUnprojected = Input.Position;
    Output.TexCoord = Input.TexCoord;
    Output.Color = Input.Color;
    return Output;
}
cbuffer PARAMS
{
    float4 Delta;
    float4 RadialBlur;      //x:fuerza
    float4 DirectionalBlur; //xy: dirección, z:fuerza
    //Add here more params as requiered
};

Texture2D Frame : register(t0);
SamplerState Sampler : register(s0);
float4 PSEdgeDetect(VERTEX_OUTPUT Input):SV_Target
{
    float4 left, right, up, down;
    left = Frame.Sample(Sampler, Input.TexCoord.xy + float2(-Delta.x, 0));
    right = Frame.Sample(Sampler, Input.TexCoord.xy + float2(Delta.x, 0));
    up = Frame.Sample(Sampler, Input.TexCoord.xy + float2(0, -Delta.y));
    down = Frame.Sample(Sampler, Input.TexCoord.xy + float2(0, Delta.y));
    return abs(right - left) + abs(up - down);
}
float4 PSRadialBlur(VERTEX_OUTPUT Input):SV_Target
{
   float r = sqrt(dot(Input.PositionUnprojected.xy, 
    Input.PositionUnprojected.xy));
   float2 Dir = normalize(Input.PositionUnprojected.xy)*float2(1,-1);
    float4 Color = 0;
    for (int i = 0; i < 8;i++)
        Color += Frame.Sample(
        Sampler, Input.TexCoord.xy + RadialBlur.x*r*Dir*i);
    return Color / 8;
}
float4 PSDirectionalBlur(VERTEX_OUTPUT Input):SV_Target
{
    float4 Color = 0;
    for (int i = 0; i < 8;i++)
        Color += Frame.Sample(Sampler,
        Input.TexCoord.xy + i * DirectionalBlur.xy * DirectionalBlur.z);
    return Color / 8;
}

float4 PSGaussHorizontalBlur(VERTEX_OUTPUT Input) : SV_Target
{
    const float Gauss[7] = { 0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006 };
    float4 Color = 0;
    for (int i = -3; i <= 3; i++)
        Color += Frame.Sample(Sampler, Input.TexCoord.xy +
        Delta.xy * float2(4*i, 0)) * Gauss[i + 3];
    return Color;
}
float4 PSGaussVerticalBlur(VERTEX_INPUT Input):SV_Target
{
    const float Gauss[7] = { 0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006 };
    float4 Color = 0;
    for (int i = -3; i <= 3; i++)
        Color += Frame.Sample(Sampler, Input.TexCoord.xy +
        Delta.xy * float2(0, 4*i)) * Gauss[i + 3];
    return Color;
}

