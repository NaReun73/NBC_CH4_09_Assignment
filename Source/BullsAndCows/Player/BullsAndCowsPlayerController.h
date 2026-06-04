#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BullsAndCowsPlayerController.generated.h"

class UBullsAndCowsChatInput;
class UUserWidget;

/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API ABullsAndCowsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABullsAndCowsPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBullsAndCowsChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBullsAndCowsChatInput> ChatInputWidgetInstance;

	// 작성한 메세지
	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};
