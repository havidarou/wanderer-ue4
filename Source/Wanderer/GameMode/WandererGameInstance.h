// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Levels/MapData.h"
#include "WandererGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WANDERER_API UWandererGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UWandererGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wanderer Map Data")
	// 1 means left, 2 means right
	int32 PlaceToEnter;
	bool Begin;
	FString CurrentMapName;

	TArray<MapData> Maps;
};
