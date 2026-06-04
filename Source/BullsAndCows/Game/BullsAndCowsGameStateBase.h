#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BullsAndCowsGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API ABullsAndCowsGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLogInMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
};
