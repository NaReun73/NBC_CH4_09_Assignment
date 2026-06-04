#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BullsAndCowsPawn.generated.h"

UCLASS()
class BULLSANDCOWS_API ABullsAndCowsPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
};
