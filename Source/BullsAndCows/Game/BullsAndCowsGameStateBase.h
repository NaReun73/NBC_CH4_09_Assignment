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
	// 다른 플레이어 로그인시 MulticastRPC
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLogInMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
};
