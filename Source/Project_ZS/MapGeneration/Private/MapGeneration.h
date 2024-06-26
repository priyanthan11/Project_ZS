// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/RandomStream.h"
#include "MapGeneration.generated.h"

UENUM(BlueprintType)
enum class ETerrainType : uint8
{
	ETT_Water UMETA(DisplayName = "Water"),
	ETT_Sand UMETA(DisplayName = "Sand"),
	ETT_Grass UMETA(DisplayName = "Grass"),
	ETT_Mountain UMETA(DisplayName = "Mountain")
};



UCLASS()
class PROJECT_ZS_API AMapGeneration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapGeneration();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	int32 MapWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	int32 MapHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	float Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	float StructureDensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	int32 Seed;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (AllowPrivateAcces = "true"))
	FRandomStream RandomStream;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (AllowPrivateAcces = "true"))
	uint32 seed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (AllowPrivateAcces = "true"))
	float RandomValue;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (AllowPrivateAcces = "true"))
	uint32 RandomInt;

	TArray<TArray<ETerrainType>> TerrainType;
private:
	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GenerteHeightMap(int32 Width, int32 Height, float m_Scale, int32 Seeds, TArray<TArray<float>>& OutHeightMap);
	void ClassifyTerrain(const TArray<TArray<float>>& p_HeightMap, TArray<TArray<ETerrainType>>& p_TerainType);
	void GenerateMap(int32 Widht, int32 Height, float m_Scale, float m_StructureDensity, int32 m_Seed);
	void DrawDebugHeightMap(const TArray<TArray<float>>& p_HeightMap, TArray<TArray<ETerrainType>>& TerrainMap, float m_Scale);
	void LODStreaming(const TArray<TArray<float>>& p_HeightMap, TArray<TArray<ETerrainType>>& TerrainMap, float m_Scale, int32 Widht, int32 Height);
	void GenerateProceduralMap(const TArray<TArray<float>>& m_HeightMap, float m_Scale);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

class PROJECT_ZS_API FPerlinNoise
{
public:
	FPerlinNoise(int32 Seed);
	float Noise(float x, float y) const;
	float NoiseAdvanced(float x, float y,float inScale, float amplitute) const;
private:
	TArray<int32>Permutation;

	float fade(float t) const;
	float Lerp(float t, float a, float b) const;
	float Grad(int32 hash, float x, float y) const;

};