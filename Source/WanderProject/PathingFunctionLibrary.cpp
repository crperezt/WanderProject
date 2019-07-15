// Fill out your copyright notice in the Description page of Project Settings.


#include "PathingFunctionLibrary.h"

#define D(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}
#define D(x,y) if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(x), y)); }

const int UPathingFunctionLibrary::rows = 6;
const int UPathingFunctionLibrary::columns = 8;

/** Array of tuples representing coordinates of each board position */
TArray<TTuple<int, int>> UPathingFunctionLibrary::allNodes;

/** Adjacency list for an undirected graph representing the board
Each array element contains a set of tuples, the set including the board positions of all neighbors to the node given by the index */
TArray<TSet<TTuple<int, int>>> UPathingFunctionLibrary::neighbors;
TArray<TTuple<int, int>> UPathingFunctionLibrary::directions;

UPathingFunctionLibrary::UPathingFunctionLibrary() {
	//Directions in which characters can move on board (north, south, east, west, and diagonals)
	directions.Add(TTuple<int, int>(0, 1));
	directions.Add(TTuple<int, int>(0, -1));
	directions.Add(TTuple<int, int>(-1, 0));
	directions.Add(TTuple<int, int>(1, 0));
	directions.Add(TTuple<int, int>(-1, 1));
	directions.Add(TTuple<int, int>(1, 1));
	directions.Add(TTuple<int, int>(-1, -1));
	directions.Add(TTuple<int, int>(1, -1));

	//initialize allNodes
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			allNodes.Add(MakeTuple(i, j));
		}
	}

	//clearAdjacencyList();
}

void UPathingFunctionLibrary::clearAdjacencyList() {
	neighbors = TArray<TSet<TTuple<int, int>>>();
	for (int i = 0; i < columns*rows; i++) {
		neighbors.Add(TSet<TTuple<int, int>>());
	}

	/*
	TTuple<int, int> newNeighbor;
	TTuple<int, int> iNode;
	const int EMPTY = 0;
	for (int i = 0; i < neighbors.Num(); i++) {
		for (int j = 0; j < directions.Num(); j++) {
			iNode = allNodes[i];
			newNeighbor = TTuple<int, int>(allNodes[i].Get<0>() + directions[j].Get<0>(), allNodes[i].Get<1>() + directions[j].Get<1>());
			if ((newNeighbor.Get<0>() < rows) && (newNeighbor.Get<0>() >= 0) && (newNeighbor.Get<1>() < columns) && (newNeighbor.Get<1>() >= 0)) {
				neighbors[i].Add(newNeighbor);
			}
		}
	}
	*/
}

/** Builds representation of the board as an undirected graph stored as an adjacency list.
Input:
occupiedNodes: array of integers representing the content of the board position given by the index. 0 means board position is empty, non-zero means board position is occupied.
Output: neighbors array is populated and excludes occupied nodes.
*/
void UPathingFunctionLibrary::updateAdjacencyList(TArray<int> occupiedNodes) {
	TTuple<int, int> newNeighbor;
	int newNeighborAsInt;
	TTuple<int, int> iNode;
	const int EMPTY = 0;

	clearAdjacencyList();

	for (int i = 0; i < neighbors.Num(); i++) {
		for (int j = 0; j < directions.Num(); j++) {
			iNode = allNodes[i];
			newNeighbor = TTuple<int, int>(allNodes[i].Get<0>() + directions[j].Get<0>(), allNodes[i].Get<1>() + directions[j].Get<1>());
			if ((newNeighbor.Get<0>() < rows) && (newNeighbor.Get<0>() >= 0) && (newNeighbor.Get<1>() < columns) && (newNeighbor.Get<1>() >= 0)) {
				newNeighborAsInt = newNeighbor.Get<0>() * 8 + newNeighbor.Get<1>();
				if (occupiedNodes[newNeighborAsInt] == EMPTY) {
					neighbors[i].Add(newNeighbor);
				}
			}
		}
	}
}  


/** Computes the position of closest reachable enemy to the character and the shortest path from the character to that enemy 
 Inputs:
 node: the position on the board of the present character
 enemyNodes: array that contains the integer board position of each enemy

 Outputs:
 Return int: position on the board of the closest reachable enemy
 pathToClosestEnemy: array that contains the board positions that form the shortest path to the closest enemy. The path goes from the last entry in the array to the first (i.e., the enemy position).
*/

int UPathingFunctionLibrary::getClosestEnemyNodeAndPath(int node, TArray<int> occupiedByAlliesNodes, TArray<int> enemyNodes, TArray<int>& pathToClosestEnemy, bool& noPathAvailable)
{
	TArray<int> discoveryState;
	TArray<int> distance;
	TArray<int> predecessor;

	for (int i = 0; i < columns*rows; i++) {
		discoveryState.Add(0);
		distance.Add(10000);
		predecessor.Add(-1);
	}
	discoveryState[node] = 1;
	distance[node] = 0;
	TArray<int> queue;
	queue.Add(node);
	int iNode;
	int nodeNumber;

	updateAdjacencyList(occupiedByAlliesNodes);

	while (queue.Num() > 0) {
		iNode = queue.Last();
		for (auto& n : neighbors[iNode]) {
			nodeNumber = n.Get<0>() * 8 + n.Get<1>(); //convert board position from tuple to integer
			if (discoveryState[nodeNumber] == 0) {
				discoveryState[nodeNumber] = 1;
				distance[nodeNumber] += 1;
				predecessor[nodeNumber] = iNode;
				queue.Insert(nodeNumber, 0);
			}
		}
		discoveryState[iNode] = 2;
		queue.Remove(iNode);
	}

	int closestEnemy;
	if (enemyNodes.IsValidIndex(0)) {
		closestEnemy = enemyNodes[0];
	}
	else {
		return -1;
	}

	if (enemyNodes.Num() > 1) {
		for (int i = 1; i < enemyNodes.Num(); i++) {
			if (distance[enemyNodes[i]] < distance[enemyNodes[i - 1]]) {
				closestEnemy = enemyNodes[i];
			}
		}
	}

	//if no path available
	if (predecessor[closestEnemy] == -1) {
		noPathAvailable = true;
		return closestEnemy;
	}
	else {
		int iPath = predecessor[closestEnemy];
		while (predecessor[iPath] != node) {
			pathToClosestEnemy.Add(predecessor[iPath]);
			iPath = predecessor[iPath];
		}
		noPathAvailable = false;
		return closestEnemy;
	}
}