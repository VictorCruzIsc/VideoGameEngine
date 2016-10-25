
// HLSL : High Level Shader Language

// Firma de entrada: Establece el formato de vértice que ingresa
// y coincide con el InputLayout que definimos en C.

struct VERTEX_INPUT
{
	float4 Position:POSITION;
	float4 Normal :   NORMAL;
    float4 Tangent : NORMAL1;
    float4 Binormal : NORMAL2;
	float4 Color:COLOR;
    float4 TexCoord : TEXCOORD;
};
struct VERTEX_OUTPUT
{
    float4 Position : SV_Position;
	float4 PositionNonProjected:POSITION;
	float4 Normal:NORMAL;
    float4 Color:COLOR;
    float4 TexCoord : TEXCOORD;
    float4 A : NORMAL1;
    float4 B : NORMAL2;
    float4 C : NORMAL3;
    float4 LightPosition : POSITION1;
};

struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    float4 Power;
};
#define LIGHT_ON 0x1
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2
struct LIGHT
{
    uint4 FlagAndType;
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Attenuation;
    float4 Position;
    float4 Direction;
    float4 Factors;
};

#define LIGHTING_AMBIENT 0x1
#define LIGHTING_DIFFUSE 0x2
#define LIGHTING_SPECULAR 0x4
#define LIGHTING_EMISSIVE 0x8
#define MAPPING_DIFFUSE  0x10
#define MAPPING_NORMAL  0x20
#define MAPPING_ENVIROMENTAL_FAST 0x40
#define MAPPING_EMISSIVE 0x80
#define MAPPING_SHADOW 0x100
#define VERTEX_PRELIGHTEN 0x200

cbuffer PARAMS:register(b0)
{
    uint4 Flags;  //Banderas de de dibujo e iluinación
    matrix World; //Model to World Trnasform
    matrix View;
    matrix Projection;
    matrix LightView;
    matrix LightProjection;
    float4 Brightness; //Pixel Shader Brightness control
    MATERIAL Material;
    LIGHT Lights[8];
};


VERTEX_OUTPUT VSMain(VERTEX_INPUT Input)
{
	VERTEX_OUTPUT Output;
    matrix WV=mul(World,View);
    matrix WVP = mul(WV, Projection);
    float4 Position = mul(Input.Position, WVP);
    Output.Position = Position;
    Output.PositionNonProjected = mul(Input.Position, WV);
    Output.Normal = normalize(mul(Input.Normal,WV));
	Output.Color = Input.Color;
    Output.TexCoord = Input.TexCoord;
    //Trasnformar el espacio tangente al espacio de vista
    float4 T, B;
    T = normalize(mul(Input.Tangent, WV));
    B = normalize(mul(Input.Binormal, WV));
    //Transponer la base orotornormal (T,B,N) ya en espacio de vista, para ir de espacio del mapa de normales a espacio de vista.
    Output.A = float4(T.x, B.x, Output.Normal.x, 0);
    Output.B = float4(T.y, B.y, Output.Normal.y, 0);
    Output.C = float4(T.z, B.z, Output.Normal.z, 0);
    Output.LightPosition = 
    mul(Input.Position, mul(World, mul(LightView, LightProjection)));
    return Output;

}
Texture2D Diffuse:register(t0);
Texture2D NormalMap : register(t1);
Texture2D EnviromentalMap : register(t2);
Texture2D EmissiveMap : register(t3);
Texture2D ShadowMap : register(t4);
SamplerState Sampler : register(s0);

float4 PSMain(VERTEX_OUTPUT Input) :SV_Target
{
    float4 ColorDiffuse = 0;
    float4 ColorSpecular = 0;
    float4 ColorEnviromental = 0;
    float4 ColorEmissive = 0;
    float4 Protuberance = 0;
    float4 N = 0;
    if(Flags.x&MAPPING_EMISSIVE)
    {
        ColorEmissive = EmissiveMap.Sample(Sampler, Input.TexCoord.xy);
    }
    if (Flags.x & MAPPING_NORMAL)
    {
        //float2 TexCoord = float2(0, 1) + Input.TexCoord.xy * float2(1, -1);
        float4 NormalSample = NormalMap.Sample(Sampler, Input.TexCoord.xy)
        *float4(2,2,1,0)-float4(1,1,0,0);
        Protuberance.x = dot(Input.A, NormalSample);
        Protuberance.y = dot(Input.B, NormalSample);
        Protuberance.z = dot(Input.C, NormalSample);
        Protuberance.w = 0;
        N=normalize(Protuberance);
    }
    else
        N = normalize(Input.Normal);
    if(Flags.x&MAPPING_ENVIROMENTAL_FAST)
    {
        ColorEnviromental = EnviromentalMap.Sample(Sampler,
        (N.xy * float2(0.5, -0.5) + 0.5));
    }
    float Shadowed = 1;
    if(Flags.x&MAPPING_SHADOW)
    {
        //1.- Calcular la coordenada de textura a partir de la posición
        // interpolada en espacio de luz.
        float4 ShadowPos = Input.LightPosition / Input.LightPosition.w;
        ShadowPos.xy = ShadowPos.xy * float2(0.5, -0.5) + 0.5;
        if(saturate(ShadowPos.x)==ShadowPos.x && 
            saturate(ShadowPos.y)==ShadowPos.y)
        {
            float depth = ShadowMap.Sample(Sampler, ShadowPos.xy).x;
            if((ShadowPos.z - depth) > 0)
                Shadowed = saturate(1-(ShadowPos.z-depth)*500);
        }
    }
    for (int i = 0; i < 8;i++)
    {
        if(Lights[i].FlagAndType.x& LIGHT_ON )
        {
            switch(Lights[i].FlagAndType.y)
            {
                case LIGHT_DIRECTIONAL:
                    {
                        float ILambert = max(0, -dot(N, Lights[i].Direction));
                        ColorDiffuse += ILambert * Lights[i].Diffuse*Shadowed;
                        float4 V = 
                        normalize(float4(0, 0, 0, 1) - Input.PositionNonProjected);
                        float4 H = normalize(V - Lights[i].Direction);
                        float IPhong = pow(max(0, dot(H, N)), Material.Power.x);
                        ColorSpecular += IPhong * Lights[i].Specular*Shadowed;
                    }
                    break;
            }
        }
    }
    if (Flags.x & MAPPING_DIFFUSE)
        ColorDiffuse *= Diffuse.Sample(Sampler, Input.TexCoord.xy);
	if (Flags.x&VERTEX_PRELIGHTEN)
		ColorEmissive += Input.Color;
    return Material.Emissive +
    ColorDiffuse * Material.Diffuse +
    ColorSpecular * Material.Specular +
    ColorEnviromental * Material.Ambient+
    Brightness+ColorEmissive;
}

float4 VSShadow(VERTEX_INPUT Input):SV_Position
{
return 
     mul(Input.Position,mul(World, mul(LightView, LightProjection)));
}

float PSShadow(float4 Position:SV_Position):SV_Target0
{
    return Position.z;
}