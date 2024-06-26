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
	Seed(0)
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
			float NoiseValue = PerlinNoise.Noise((x * 0.0001f) / Scale, (y*0.0001f) / Scale);


			OutHeightMap[x][y] = NoiseValue;
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
			float HeightValue = m_HeightMap[x][y] * 350.f; // Z scale of the map
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

float FPerlinNoise::Grad(int32 hash, float x, float y) const
{
	int32 h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0.0f);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);

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
float FPerlinNoise::NoiseAdvanced(float x, float y, float inScale, float amplitute) const
{

	float px = (x * inScale) + 0.1f;
	float py = (y * inScale) + 0.1f;

	float adNoise = Noise(px, py);

	return adNoise * amplitute;
}
#pragma endregion