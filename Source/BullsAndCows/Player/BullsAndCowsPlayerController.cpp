#include "BullsAndCowsPlayerController.h"
#include "UI/BullsAndCowsChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BullsAndCows/BullsAndCows.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BullsAndCowsGameModeBase.h"
#include "BullsAndCowsPlayerState.h"
#include "Net/UnrealNetwork.h"

ABullsAndCowsPlayerController::ABullsAndCowsPlayerController()
{
	bReplicates = true;
}

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

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ABullsAndCowsPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = GetPlayerState<ABullsAndCowsPlayerState>();
		if (IsValid(BullsAndCowsPlayerState) == true)
		{
			// 현재 플레이어 정보 더하기 (정답 시도시 바로 반영 X)
			//FString CombinedMessageString = BullsAndCowsPlayerState->GetPlayerInfoString() + TEXT(" : ") + InChatMessageString;
			//ServerRPCPrintChatMessageString(CombinedMessageString);

			// 정답 시도시 바로 반영 처리해보기
			ServerRPCPrintChatMessageString(InChatMessageString);
		}
	}
}

void ABullsAndCowsPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	BullsAndCowsFuntionLibrary::MyPrintString(this, InChatMessageString, 10.0f);
}

void ABullsAndCowsPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
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

