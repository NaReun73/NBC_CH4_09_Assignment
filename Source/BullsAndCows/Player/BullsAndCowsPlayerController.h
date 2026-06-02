#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BullsAndCowsPlayerController.generated.h"

class UBullsAndCowsChatInput;
/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API ABullsAndCowsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBullsAndCowsChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBullsAndCowsChatInput> ChatInputWidgetInstance;

	// 작성한 메세지
	FString ChatMessageString;
};
