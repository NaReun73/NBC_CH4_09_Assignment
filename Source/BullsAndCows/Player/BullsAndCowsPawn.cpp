#include "Player/BullsAndCowsPawn.h"
#include "BullsAndCows.h"

void ABullsAndCowsPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FString NetRoleString = BullsAndCowsFuntionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BullsAndCowsPawn::BeginPlay() %s [%s]"), *BullsAndCowsFuntionLibrary::GetNetModeString(this), *NetRoleString);
	BullsAndCowsFuntionLibrary::MyPrintString(this, CombinedString, 10.0f);
}

void ABullsAndCowsPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = BullsAndCowsFuntionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BullsAndCowsPawn::PossessedBy() %s [%s]"), *BullsAndCowsFuntionLibrary::GetNetModeString(this), *NetRoleString);
	BullsAndCowsFuntionLibrary::MyPrintString(this, CombinedString, 10.0f);
}

