//--------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Константные буферы
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;			// Матрица мира
	matrix View;			// Матрица вида
	matrix Projection;		// Матрица проекции
	float4 vLightDir[2];	// Направление источника света
	float4 vLightColor[2];	// Цвет источника света
	float4 vOutputColor;	// Активный цвет
}


struct VS_DATA					// Входящие данные вершинного шейдера
{
	float3 Normal;		// Позиция по X, Y, Z
	float4 Color;		// Нормаль по X, Y, Z
};


cbuffer InfoBuffer: register(b1)
{
	VS_DATA BufferData[6];
};


//--------------------------------------------------------------------------------------
struct VS_INPUT					// Входящие данные вершинного шейдера
{
    float4 Pos : POSITION;		// Позиция по X, Y, Z
    int index : NORMAL;		// Нормаль по X, Y, Z
};

struct PS_INPUT					// Входящие данные пиксельного шейдера
{
    float4 Pos : SV_POSITION;	// Позиция пикселя в проекции (экранная)
    int Norm : TEXCOORD0;	// Относительная нормаль пикселя по tu, tv
	float4 Color: COLOR;
};


//--------------------------------------------------------------------------------------
// Вершинный шейдер
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
// Пиксельный шейдер для куба
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{

	//return input.Color;
	float4 finalColor = 0;// nput.Color;
	// складываем освещенность пикселя от всех источников света
	for(int i=0; i<2; i++)
	 {
	     finalColor += saturate( dot( (float3)vLightDir[i], input.Norm) * vLightColor[i] );
	 }
	 finalColor.a = 1;
	 return finalColor;
}


//--------------------------------------------------------------------------------------
// Пиксельный шейдер для источников света
//--------------------------------------------------------------------------------------
float4 PSSolid( PS_INPUT input) : SV_Target
{
    return vOutputColor;
}
