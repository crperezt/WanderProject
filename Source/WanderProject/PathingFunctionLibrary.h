// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathingFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WANDERPROJECT_API UPathingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 
	UPathingFunctionLibrary();

	static const int rows;
	static const int columns;
	static TArray<TTuple<int, int>> allNodes;
	static TArray<TSet<TTuple<int, int>>> neighbors;
	static TArray<TTuple<int, int>> directions;

	UFUNCTION(BlueprintCallable)
	static void updateAdjacencyList(TArray<int> occupiedNodes);
	
	UFUNCTION(BlueprintCallable)
	static int getClosestEnemyNodeAndPath(int node, TArray<int> enemyNodes, TArray<int>& pathToClosestEnemy);

private:
	
};
