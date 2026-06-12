#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BullsAndCowsGameStateBase.generated.h"

class ABullsAndCowsPlayerState;

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	End
};

/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API ABullsAndCowsGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 다른 플레이어 로그인시 MulticastRPC
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLogInMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

protected:
	//UFUNCTION()
	//void OnRep_CurrentTurnPlayer();

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 AlivePlayerControllerCount = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EMatchState MatchState = EMatchState::Waiting;

	//Using = OnRep_CurrentTurnPlayer
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ABullsAndCowsPlayerState> CurrentTurnPlayerState;

	int32 TurnRemainingTime = 30;

};
