#include "Player/BullsAndCowsPlayerState.h"
#include "Net/UnrealNetwork.h"

ABullsAndCowsPlayerState::ABullsAndCowsPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void ABullsAndCowsPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ABullsAndCowsPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT(" (") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");

	return PlayerInfoString;
}
