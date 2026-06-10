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

	// 정답 번호 생성
	FString GenerateSecretNumber();

	// 일반 채팅인지 문제의 답인지 확인
	bool IsGuessNumberString(const FString& InNumberString);

	// 정답 비교
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ABullsAndCowsPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ABullsAndCowsPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ABullsAndCowsPlayerController* InChattingPlayerController, int InStrikeCount);

private:
	UFUNCTION()
	void OnMainTimerElapsed();

	void NotifyToAllPlayer(const FString& NotificationString);

public:
	FTimerHandle MainTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 10;

	int32 RemainWaitingTimeForPlaying = 10;

	int32 MinimumPlayerCountForPlaying = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PlayTimeLimit = 30;

	int32 PlayTime = 30;

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ABullsAndCowsPlayerController>> AllPlayerControllers;
};
