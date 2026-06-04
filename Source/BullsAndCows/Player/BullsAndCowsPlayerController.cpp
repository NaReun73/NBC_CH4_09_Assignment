#include "BullsAndCowsPlayerController.h"
#include "UI/BullsAndCowsChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BullsAndCows/BullsAndCows.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BullsAndCowsGameModeBase.h"

void ABullsAndCowsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UBullsAndCowsChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
}

void ABullsAndCowsPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}

void ABullsAndCowsPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	BullsAndCowsFuntionLibrary::MyPrintString(this, InChatMessageString, 10.0f);
}

void ABullsAndCowsPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ABullsAndCowsPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABullsAndCowsGameModeBase* BullsAndCowsGM = Cast<ABullsAndCowsGameModeBase>(GM);
		if (IsValid(BullsAndCowsGM) == true)
		{
			BullsAndCowsGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

