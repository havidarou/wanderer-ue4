// Fill out your copyright notice in the Description page of Project Settings.

#include "Wanderer.h"
#include "MapData.h"

MapData::MapData()
{
}

MapData::MapData(FString Name, TArray<FVector> EntryPoints, TArray<FString> AdyacentMaps, int CloseEnemies, int RangedEnemies, int HealingEnemies)
{
	this->Name = Name;
	this->EntryPoints = EntryPoints;
	this->AdyacentMaps = AdyacentMaps;
	this->CloseEnemies = CloseEnemies;
	this->RangedEnemies = RangedEnemies;
	this->HealingEnemies = HealingEnemies;
}

MapData::~MapData()
{
}
