//--------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// ����������� ������
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;			// ������� ����
	matrix View;			// ������� ����
	matrix Projection;		// ������� ��������
	float4 vLightDir[2];	// ����������� ��������� �����
	float4 vLightColor[2];	// ���� ��������� �����
	float4 vOutputColor;	// �������� ����
}


struct VS_DATA					// �������� ������ ���������� �������
{
	float3 Normal;		// ������� �� X, Y, Z
	float4 Color;		// ������� �� X, Y, Z
};


cbuffer InfoBuffer: register(b1)
{
	VS_DATA BufferData[6];
};


//--------------------------------------------------------------------------------------
struct VS_INPUT					// �������� ������ ���������� �������
{
    float4 Pos : POSITION;		// ������� �� X, Y, Z
    int index : NORMAL;		// ������� �� X, Y, Z
};

struct PS_INPUT					// �������� ������ ����������� �������
{
    float4 Pos : SV_POSITION;	// ������� ������� � �������� (��������)
    int Norm : TEXCOORD0;	// ������������� ������� ������� �� tu, tv
	float4 Color: COLOR;
};


//--------------------------------------------------------------------------------------
// ��������� ������
//--------------------------------------------------------------------------------------
PS_INPUT VS(float4 Pos : POSITION, int index : NORMAL)
{
	VS_DATA tm = BufferData[index];

    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Norm = mul( tm.Normal, World );
	output.Color = tm.Color;
    
    return output;
}


//--------------------------------------------------------------------------------------
// ���������� ������ ��� ����
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{

	//return input.Color;
	float4 finalColor = 0;// nput.Color;
	// ���������� ������������ ������� �� ���� ���������� �����
	for(int i=0; i<2; i++)
	 {
	     finalColor += saturate( dot( (float3)vLightDir[i], input.Norm) * vLightColor[i] );
	 }
	 finalColor.a = 1;
	 return finalColor;
}


//--------------------------------------------------------------------------------------
// ���������� ������ ��� ���������� �����
//--------------------------------------------------------------------------------------
float4 PSSolid( PS_INPUT input) : SV_Target
{
    return vOutputColor;
}
