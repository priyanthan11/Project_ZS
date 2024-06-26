// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_ZS/MapGeneration/Private/MapGeneration.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"




// Sets default values
AMapGeneration::AMapGeneration():
	MapWidth(100),
	MapHeight(100),
	Scale(50.f),
	StructureDensity(0.02f),
	Seed(0),
	GOctaves(4)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMesh->SetupAttachment(RootComponent);
	if (Seed == 0) Seed = FMath::Rand();
	
}

// Called when the game starts or when spawned
void AMapGeneration::BeginPlay()
{
	Super::BeginPlay();
	
	//GenerteHeightMap(10, 10, 50, seed, HeightMap);
	GenerateMap(MapWidth, MapHeight, Scale, StructureDensity, Seed);
}

void AMapGeneration::GenerteHeightMap(int32 Width, int32 Height, float m_Scale, int32 Seeds, TArray<TArray<float>>& OutHeightMap)
{
	FPerlinNoise PerlinNoise(Seed);

	// Resize the outHeightMap to the correct dimensions
	OutHeightMap.SetNum(Width);
	for (int32 x = 0; x < Width; x++)
	{
		OutHeightMap[x].SetNum(Height);
	}

	// Fill the outHeightMap with Perlin noise values
	for (int32 x = 0; x < Width; x++)
	{
		for (int32 y = 0; y < Height; y++)
		{

			//float NoiseValue = PerlinNoise.Noise(x/Scale , y /Scale);
			float NoiseValue = PerlinNoise.FractalNoise3D(x / Scale, y / Scale,0.0f, 4, 0.4f,5.5f);
			// Add sine curve variation
			float SineValue = /*SineAmplitude*/0.8f * FMath::Sin(/*SineFrequency*/0.05f * x) * FMath::Sin(/*SineFrequency*/0.05f * y);
			float SineValue2 = /*SineAmplitude*/0.4f * FMath::Sin(/*SineFrequency*/0.03f * x) * FMath::Sin(/*SineFrequency*/0.06f * y);
			float SineValue3 = /*SineAmplitude*/0.2f * FMath::Sin(/*SineFrequency*/0.09f * x) * FMath::Sin(/*SineFrequency*/0.09f * y);

			OutHeightMap[x][y] = NoiseValue + (SineValue+ SineValue2+ SineValue3);
		}
	}
	// Debug log to confirm height map generation
	if (OutHeightMap.Num() == Width && OutHeightMap[0].Num() == Height)
	{
		UE_LOG(LogTemp, Warning, TEXT("HeightMap successfully generated with dimensions: %d x %d"), Width, Height);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate HeightMap with dimensions: %d x %d"), Width, Height);
	}

}

void AMapGeneration::ClassifyTerrain(const TArray<TArray<float>>& p_HeightMap, TArray<TArray<ETerrainType>>& p_TerainType)
{

	int32 Width = p_HeightMap.Num();
	int32 Height = p_HeightMap[0].Num();

	p_TerainType.SetNum(Width);
	for (int32 x = 0; x < Width; ++x)
	{
		p_TerainType[x].SetNum(Height);
		for (int32 y = 0; y < Height; ++y)
		{
			float HeightValue = p_HeightMap[x][y];
			if (HeightValue < -0.2f)
			{
				p_TerainType[x][y] = ETerrainType::ETT_Water;
			}
			else if (HeightValue < 0.0f)
			{
				p_TerainType[x][y] = ETerrainType::ETT_Sand;
			}
			else if (HeightValue < 0.5f)
			{
				p_TerainType[x][y] = ETerrainType::ETT_Grass;
			}
			else
			{
				p_TerainType[x][y] = ETerrainType::ETT_Mountain;
			}
		}
	}


}

void AMapGeneration::GenerateMap(int32 Widht, int32 Height, float m_Scale, float m_StructureDensity, int32 m_Seed)
{

	TArray<TArray<float>> m_HeightMap;
	GenerteHeightMap(Widht, Height, Scale, Seed, m_HeightMap);

	TArray<TArray<ETerrainType>> TerrainMap;
	ClassifyTerrain(m_HeightMap, TerrainMap);

	// Structure and detail generation can be added here


	
	// Draw debug points for the height map
	//DrawDebugHeightMap(m_HeightMap, TerrainMap, Scale);
	
	// Generate the procedural mesh
	GenerateProceduralMap(m_HeightMap, Scale);
	//LODStreaming(m_HeightMap, TerrainMap, Scale, Widht, Height);

}

void AMapGeneration::DrawDebugHeightMap(const TArray<TArray<float>>& m_HeightMap, TArray<TArray<ETerrainType>>& TerrainMap,float m_Scale)
{

	for (int32 x = 0; x < m_HeightMap.Num(); ++x)
	{
		for (int32 y = 0; y < m_HeightMap[x].Num(); ++y)
		{
			float HeightValue = m_HeightMap[x][y];
			FVector PointLocation = FVector(x * Scale, y * Scale, HeightValue * 100.0f); // Adjust Z scale as needed

			// Determine color based on terrain type
			FColor PointColor;
			switch (TerrainMap[x][y])
			{
			case ETerrainType::ETT_Water:
				PointColor = FColor::Blue;
				break;
			case ETerrainType::ETT_Sand:
				PointColor = FColor::Yellow;
				break;
			case ETerrainType::ETT_Grass:
				PointColor = FColor::Green;
				break;
			case ETerrainType::ETT_Mountain:
				PointColor = FColor::Black;
				break;
			default:
				PointColor = FColor::White;
				break;
			}




			// Draw the debug point
			DrawDebugPoint(GetWorld(), PointLocation, 5.0f, PointColor, true, -1.0f, 0);

			// Optional: Draw connecting lines for better visualization
			if (x > 0)
			{
				FVector PreviousPointLocation = FVector((x - 1) * Scale, y * Scale, m_HeightMap[x - 1][y] * 100.0f);
				DrawDebugLine(GetWorld(), PreviousPointLocation, PointLocation, FColor::Red, true, -1.0f, 0, 1.0f);
			}
			if (y > 0)
			{
				FVector PreviousPointLocation = FVector(x * Scale, (y - 1) * Scale, m_HeightMap[x][y - 1] * 100.0f);
				DrawDebugLine(GetWorld(), PreviousPointLocation, PointLocation, FColor::Red, true, -1.0f, 0, 1.0f);
			}

		}
	}
}

void AMapGeneration::LODStreaming(const TArray<TArray<float>>& p_HeightMap, TArray<TArray<ETerrainType>>& TerrainMap, float m_Scale, int32 Widht, int32 Height)
{

	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	int32 PlayerX = FMath::RoundToInt(PlayerLocation.X / Scale);
	int32 PlayerY = FMath::RoundToInt(PlayerLocation.Y / Scale);
	int32 VidewDistance = 1000; // View Distance

	for (int32 x = PlayerX - VidewDistance; x < PlayerX + VidewDistance; x++)
	{
		for (int32 y = PlayerY - VidewDistance; x < PlayerY + VidewDistance; y++)
		{
			if (x >= 0 && x < Widht && y >= 0 && y < Height)
			{
				float HeightValue = p_HeightMap[x][y];
				FVector PointLocation = FVector(x * Scale, y * Scale, HeightValue * 100.0f); // Adjust Z scale as needed

				// Determine color based on terrain type
				FColor PointColor;
				switch (TerrainMap[x][y])
				{
				case ETerrainType::ETT_Water:
					PointColor = FColor::Blue;
					break;
				case ETerrainType::ETT_Sand:
					PointColor = FColor::Yellow;
					break;
				case ETerrainType::ETT_Grass:
					PointColor = FColor::Green;
					break;
				case ETerrainType::ETT_Mountain:
					PointColor = FColor::Black;
					break;
				default:
					PointColor = FColor::White;
					break;
				}

				// Draw the debug point
				DrawDebugPoint(GetWorld(), PointLocation, 5.0f, PointColor, true, -1.0f, 0);
			}
		}
	}

}

void AMapGeneration::GenerateProceduralMap(const TArray<TArray<float>>& m_HeightMap, float m_Scale)
{
	
	int32 width = m_HeightMap.Num();
	if (width == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HeightMap is empty! Cannot generate procedural mesh."));
		return;
	}

	int32 height = m_HeightMap[0].Num();
	if (height == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HeightMap row is empty! Cannot generate procedural mesh."));
		return;
	}


	TArray<FVector> Verticies;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColor;
	TArray<FProcMeshTangent> Tangents;


	

	//Generate verticies
	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			float HeightValue = m_HeightMap[x][y] * 850.f; // Z scale of the map
			Verticies.Add(FVector(x * m_Scale, y * m_Scale, HeightValue));
			Normals.Add(FVector(0,0,1)); // Default Normals

			FVector2D UV;
			UV.X = (float)x / (float)(width - 1);
			UV.Y = (float)y / (float)(height - 1);
			UVs.Add(UV);

			VertexColor.Add(FColor::White);
			Tangents.Add(FProcMeshTangent(1, 0, 0)); // Default tangent

		}
	}

	// Generate triangles
	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{

			if (x < width -1 && y < height -1)
			{

				int32 BottomLeft = x + y * width;
				int32 BottemRight = (x + 1) + y * width;
				int32 TopLeft = x + (y + 1) * width;
				int32 TopRight = (x + 1) + (y + 1) * width;


				// First Triangle
				Triangles.Add(BottomLeft);
				Triangles.Add(TopLeft);
				Triangles.Add(TopRight);

				// Second Trinagle
				Triangles.Add(BottomLeft);
				Triangles.Add(TopRight);
				Triangles.Add(BottemRight);

				/*int32 Index = x + y * width;
				Triangles.Add(Index);
				Triangles.Add(Index + width + 1);
				Triangles.Add(Index + width);


				Triangles.Add(Index + 1);
				Triangles.Add(Index + 1);
				Triangles.Add(Index + width + 1);*/
			}



			
		}
	}
	// Create the Mesh
	ProceduralMesh->CreateMeshSection(0, Verticies, Triangles, Normals, UVs, VertexColor, Tangents, true);
	//ProceduralMesh->CreateMeshSection_LinearColor(0,Verticies,Triangles,Normals,UVs,VertexColor,Tangents,true);


}

// Called every frame
void AMapGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


#pragma region PerlinNoise

FPerlinNoise::FPerlinNoise(int32 Seed)
{
	Permutation.SetNum(512);
	TArray<int32> p;
	p.SetNum(256);

	FRandomStream RandomStram(Seed);

	for (int32 i = 0; i < 256; i++)
	{
		p[i];
	}

	for (int32 i = 255; i >0; i--)
	{
		int32 SwapIndex = RandomStram.RandRange(0, i);
		int32 Temp = p[i];
		p[i] = p[SwapIndex];
		p[SwapIndex] = Temp;
	}

	for (int32 i = 0; i < 256; i++)
	{
		Permutation[i] = Permutation[i + 256] = p[i];
	}

}



float FPerlinNoise::fade(float t) const
{
	return t*t*t*(t*(t*6 -15)+10);
}

float FPerlinNoise::Lerp(float t, float a, float b) const
{
	return a+t*(b-a);
}

float FPerlinNoise::Grad1D(int32 hash, float x) const
{
	int32 H = hash & 15;
	float Grad = 1 + (H & 7); // Gradient value is one of 1, 2, ..., 8
	if ((H & 8) != 0) Grad = -Grad; // and a random sign
	return Grad * x;
}

float FPerlinNoise::Grad(int32 hash, float x, float y) const
{
	int32 h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0.0f);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);

}

float FPerlinNoise::Grad3D(int32 hash, float x, float y, float z) const
{
	int32 H = hash & 15;
	float U = H < 8 ? x : y;
	float V = H < 4 ? y : (H == 12 || H == 14 ? x : z);
	return ((H & 1) == 0 ? U : -U) + ((H & 2) == 0 ? V : -V);
}

float FPerlinNoise::OctaveNoise(float x, float y, int32 Octaves, float Presistence) const
{
	float Total = 0;
	float Frequency = 1;
	float Amplitude = 1;
	float MaxValue = 0;

	for (int32 i = 0; i < Octaves; i++)
	{
		Total += Noise(x * Frequency, y * Frequency) * Amplitude;
		MaxValue += Amplitude;

		Amplitude *= Presistence;
		Frequency *= 2;
	}


	return Total/MaxValue;
}

float FPerlinNoise::OctaveNoise3D(float x, float y, float z, int32 Octaves, float Presistence) const
{
	float Total = 0;
	float Frequency = 1;
	float Amplitude = 1;
	float MaxValue = 0;

	for (int32 i = 0; i < Octaves; ++i)
	{
		Total += Noise3D(x * Frequency, y * Frequency, z * Frequency) * Amplitude;

		MaxValue += Amplitude;

		Amplitude *= Presistence;
		Frequency *= 2;
	}

	return Total / MaxValue;
}

float FPerlinNoise::OctaveNoise1D(float x, int32 Octaves, float Presistence) const
{
	float Total = 0;
	float Frequency = 1;
	float Amplitude = 1;
	float MaxValue = 0;

	for (int32 i = 0; i < Octaves; ++i)
	{
		Total += Noise1D(x * Frequency) * Amplitude;

		MaxValue += Amplitude;

		Amplitude *= Presistence;
		Frequency *= 2;
	}

	return Total / MaxValue;
}

float FPerlinNoise::FractalNoise3D(float X, float Y, float Z, int32 Octaves, float Persistence, float Lacunarity) const
{
	float Total = 0;
	float Frequency = 1;
	float Amplitude = 1;
	float MaxValue = 0;

	for (int32 i = 0; i < Octaves; ++i)
	{
		Total += Noise3D(X * Frequency, Y * Frequency, Z * Frequency) * Amplitude;

		MaxValue += Amplitude;

		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}

	return Total / MaxValue;
}

float FPerlinNoise::Noise(float x, float y) const
{

	int32 X = FMath::FloorToInt(x) & 255;
	int32 Y = FMath::FloorToInt(y) & 255;

	x -= FMath::FloorToInt(x);
	y -= FMath::FloorToInt(y);

	float u = fade(x);
	float v = fade(y);

	int32 A = Permutation[X] + Y;
	int32 AA = Permutation[A];
	int32 AB = Permutation[A + 1];
	int32 B = Permutation[X + 1] + Y;
	int32 BA = Permutation[B];
	int32 BB = Permutation[B + 1];

	return Lerp(v, Lerp(u, Grad(Permutation[AA], x, y), Grad(Permutation[BA], x - 1, y)),
		Lerp(u, Grad(Permutation[AB], x, y - 1), Grad(Permutation[BA], x - 1, y - 1)));


}
float FPerlinNoise::Noise3D(float x, float y, float z) const
{
	int32 X0 = FMath::FloorToInt(x) & 255;
	int32 Y0 = FMath::FloorToInt(y) & 255;
	int32 Z0 = FMath::FloorToInt(z) & 255;
	int32 X1 = (X0 + 1) & 255;
	int32 Y1 = (Y0 + 1) & 255;
	int32 Z1 = (Z0 + 1) & 255;

	float Xf = x - FMath::FloorToFloat(x);
	float Yf = y - FMath::FloorToFloat(y);
	float Zf = z - FMath::FloorToFloat(z);

	float U = fade(Xf);
	float V = fade(Yf);
	float W = fade(Zf);

	int32 AAA = Permutation[Permutation[Permutation[X0] + Y0] + Z0];
	int32 ABA = Permutation[Permutation[Permutation[X0] + Y1] + Z0];
	int32 AAB = Permutation[Permutation[Permutation[X0] + Y0] + Z1];
	int32 ABB = Permutation[Permutation[Permutation[X0] + Y1] + Z1];
	int32 BAA = Permutation[Permutation[Permutation[X1] + Y0] + Z0];
	int32 BBA = Permutation[Permutation[Permutation[X1] + Y1] + Z0];
	int32 BAB = Permutation[Permutation[Permutation[X1] + Y0] + Z1];
	int32 BBB = Permutation[Permutation[Permutation[X1] + Y1] + Z1];

	float GradAAA = Grad3D(AAA, Xf, Yf, Zf);
	float GradBAA = Grad3D(BAA, Xf - 1, Yf, Zf);
	float GradABA = Grad3D(ABA, Xf, Yf - 1, Zf);
	float GradBBA = Grad3D(BBA, Xf - 1, Yf - 1, Zf);
	float GradAAB = Grad3D(AAB, Xf, Yf, Zf - 1);
	float GradBAB = Grad3D(BAB, Xf - 1, Yf, Zf - 1);
	float GradABB = Grad3D(ABB, Xf, Yf - 1, Zf - 1);
	float GradBBB = Grad3D(BBB, Xf - 1, Yf - 1, Zf - 1);

	float LerpX1 = Lerp(U, GradAAA, GradBAA);
	float LerpX2 = Lerp(U, GradABA, GradBBA);
	float LerpY1 = Lerp(V, LerpX1, LerpX2);

	float LerpX3 = Lerp(U, GradAAB, GradBAB);
	float LerpX4 = Lerp(U, GradABB, GradBBB);
	float LerpY2 = Lerp(V, LerpX3, LerpX4);

	return Lerp(W, LerpY1, LerpY2);
}
float FPerlinNoise::Noise1D(float x) const
{
	int32 x0 = FMath::FloorToInt(x) & 255;
	int32 x1 = (x0 + 1) & 255;

	float xf = x - FMath::FloorToInt(x);
	float u = fade(xf);

	int32 a = Permutation[x0];
	int32 b = Permutation[x1];

	float GradA = Grad1D(a, xf);
	float GradB = Grad1D(b, x1);

	return Lerp(u, GradA, GradB);
}
float FPerlinNoise::NoiseAdvanced(float x, float y, float inScale, float amplitute) const
{

	float px = (x * inScale) + 0.1f;
	float py = (y * inScale) + 0.1f;

	float adNoisex = Noise1D(px);
	float adNoisey = Noise1D(py);

	return (adNoisex*adNoisey)*amplitute;
}
#pragma endregion