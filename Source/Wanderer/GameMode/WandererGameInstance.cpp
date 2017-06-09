// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "WandererGameInstance.h"

UWandererGameInstance::UWandererGameInstance()
{
	PlaceToEnter = 1;
	CurrentMapName = "StartingMap";
	Begin = true;

	// Auxiliar arrays for filling in the map graph
	TArray<FVector> EntryPointsAux;
	TArray<FString> AdyacentMapsAux;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Map graph

	EntryPointsAux.Add(FVector(-2450.0f, -481.0f, 30.0f));
	EntryPointsAux.Add(FVector(1750.0f, -481.0f, 90.0f));
	AdyacentMapsAux.Add("No");
	AdyacentMapsAux.Add("StartingMap2");
	Maps.Add(MapData(FString("StartingMap"), EntryPointsAux, AdyacentMapsAux, 2, 1, 1));

	EntryPointsAux.Empty();
	AdyacentMapsAux.Empty();

	EntryPointsAux.Add(FVector(-2450.0f, -481.0f, 30.0f));
	EntryPointsAux.Add(FVector(1750.0f, -481.0f, 90.0f));
	AdyacentMapsAux.Add("StartingMap");
	AdyacentMapsAux.Add("No");
	Maps.Add(MapData(FString("StartingMap2"), EntryPointsAux, AdyacentMapsAux, 0, 0, 0));
}
