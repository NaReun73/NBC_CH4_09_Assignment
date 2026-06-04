#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BullsAndCowsGameModeBase.generated.h"


class ABullsAndCowsPlayerController;

/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API ABullsAndCowsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ABullsAndCowsPlayerController* InChattingPlayerController, const FString& InChatMessageString);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ABullsAndCowsPlayerController>> AllPlayerControllers;
};
