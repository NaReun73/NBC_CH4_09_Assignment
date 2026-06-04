#include "BullsAndCowsGameModeBase.h"
#include "BullsAndCowsGameStateBase.h"
#include "Player/BullsAndCowsPlayerController.h"
#include "EngineUtils.h"

void ABullsAndCowsGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
	if (IsValid(BullsAndCowsGameStateBase) == true)
	{
		BullsAndCowsGameStateBase->MulticastRPCBroadcastLogInMessage(TEXT("XXXXXXX"));
	}

	ABullsAndCowsPlayerController* BullsAndCowsPlayerController = Cast<ABullsAndCowsPlayerController>(NewPlayer);
	if (IsValid(BullsAndCowsPlayerController) == true)
	{
		AllPlayerControllers.Add(BullsAndCowsPlayerController);
	}
}

FString ABullsAndCowsGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) {return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; i++)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABullsAndCowsGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {
		// 3자리 확인
		if (InNumberString.Len() != 3)
		{
			break;
		}

		// 문자가 섞여있나 확인
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		// 중복 숫자 확인
		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ABullsAndCowsGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; i++)
	{
		// 자리도 같고 숫자도 같다면
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			// 자리는 틀리지만 숫자가 포함되어있다면
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	// 해당 숫자가 없다면
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%d Strike - %d Ball"), StrikeCount, BallCount);

}

void ABullsAndCowsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void ABullsAndCowsGameModeBase::PrintChatMessageString(ABullsAndCowsPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;

	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		for (TActorIterator<ABullsAndCowsPlayerController> It(GetWorld()); It; ++It)
		{
			ABullsAndCowsPlayerController* BullsAndCowsPlayerController = *It;
			if (IsValid(BullsAndCowsPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				BullsAndCowsPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
	}
	else
	{
		for (TActorIterator<ABullsAndCowsPlayerController> It(GetWorld()); It; ++It)
		{
			ABullsAndCowsPlayerController* BullsAndCowsPlayerController = *It;
			if (IsValid(BullsAndCowsPlayerController) == true)
			{
				BullsAndCowsPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}
