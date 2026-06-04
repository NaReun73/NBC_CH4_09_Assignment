#include "Game/BullsAndCowsGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BullsAndCowsPlayerController.h"

void ABullsAndCowsGameStateBase::MulticastRPCBroadcastLogInMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABullsAndCowsPlayerController* BullsAndCowsPC = Cast<ABullsAndCowsPlayerController>(PC);
			if (IsValid(BullsAndCowsPC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the game");
				BullsAndCowsPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
