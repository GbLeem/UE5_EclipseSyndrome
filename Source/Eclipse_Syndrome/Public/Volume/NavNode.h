#pragma once

#include "CoreMinimal.h"
#include <vector>

class ECLIPSE_SYNDROME_API NavNode
{
public:
	FIntVector Coordinates;

	std::vector<NavNode*> Neighbors;

	float FScore = FLT_MAX;
};

struct NodeCompare
{
	bool operator() (const NavNode* lhs, const NavNode* rhs) const
	{
		return (lhs->FScore < rhs->FScore);
	}
};
