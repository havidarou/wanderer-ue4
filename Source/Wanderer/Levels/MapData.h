// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class WANDERER_API MapData
{
public:
	MapData();
	MapData(FString Name, TArray<FVector> EntryPoints, TArray<FString> AdyacentMaps, int CloseEnemies, int RangedEnemies, int HealingEnemies);
	~MapData();

	// First entry point, is the left one
	// Second entry point, is the right one
	TArray<FVector> EntryPoints;
	// First name, is the left one
	// Second name, is the right one
	TArray<FString> AdyacentMaps;
	FString Name;
	int CloseEnemies;
	int RangedEnemies;
	int HealingEnemies;
};
