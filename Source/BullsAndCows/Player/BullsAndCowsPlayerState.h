#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BullsAndCowsPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API ABullsAndCowsPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABullsAndCowsPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();

public:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
};
