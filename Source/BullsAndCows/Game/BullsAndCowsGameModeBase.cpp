#include "BullsAndCowsGameModeBase.h"
#include "BullsAndCowsGameStateBase.h"
#include "Player/BullsAndCowsPlayerController.h"
#include "EngineUtils.h"
#include "Player/BullsAndCowsPlayerState.h"

void ABullsAndCowsGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABullsAndCowsPlayerController* BullsAndCowsPlayerController = Cast<ABullsAndCowsPlayerController>(NewPlayer);
	if (IsValid(BullsAndCowsPlayerController) == true)
	{
		BullsAndCowsPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BullsAndCowsPlayerController);

		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = BullsAndCowsPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
		if (IsValid(BullsAndCowsPlayerState) == true)
		{
			BullsAndCowsPlayerState->PlayerNameString = TEXT("Player ") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
		if (IsValid(BullsAndCowsGameStateBase) == true)
		{
			BullsAndCowsGameStateBase->MulticastRPCBroadcastLogInMessage(BullsAndCowsPlayerState->PlayerNameString);
		}
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
			// 문자가 섞여있거나 0일 경우
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			// 검사 이후 Set에 넣어주기
			UniqueDigits.Add(C);
		}

		// 중복이 있으면 3자리가 안됨
		if (UniqueDigits.Num() < 3)
		{
			break;
		}

		// 중복 숫자 확인
		/*if (bIsUnique == false)
		{
			break;
		}*/

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

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.0f, true);

	RemainWaitingTimeForPlaying = WaitingTime;

	PlayTime = PlayTimeLimit;
}

void ABullsAndCowsGameModeBase::PrintChatMessageString(ABullsAndCowsPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	// 기존 강의에서 뒤에서 3자리를 확인
	//int Index = InChatMessageString.Len() - 3;
	//FString GuessNumberString = InChatMessageString.RightChop(Index);

	// 메시지 그대로 확인
	FString GuessNumberString = InChatMessageString;

	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = InChattingPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
		if (IsValid(BullsAndCowsPlayerState) == true)
		{
			FString CombinedMessageString = BullsAndCowsPlayerState->GetPlayerInfoString() + +TEXT(" : ") + InChatMessageString;

			for (TActorIterator<ABullsAndCowsPlayerController> It(GetWorld()); It; ++It)
			{
				ABullsAndCowsPlayerController* BullsAndCowsPlayerController = *It;
				if (IsValid(BullsAndCowsPlayerController) == true)
				{
					FString FinalCombinedMessageString = CombinedMessageString + TEXT(" -> ") + JudgeResultString;
					BullsAndCowsPlayerController->ClientRPCPrintChatMessageString(FinalCombinedMessageString);

					int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
					JudgeGame(InChattingPlayerController, StrikeCount);
				}
			}
		}
	}
	else
	{
		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = InChattingPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
		if (IsValid(BullsAndCowsPlayerState) == true)
		{
			FString CombinedMessageString = BullsAndCowsPlayerState->GetPlayerInfoString() + +TEXT(" : ") + InChatMessageString;

			if (GuessNumberString.Len() == 3)
			{
				CombinedMessageString = CombinedMessageString + TEXT(" (중복된 숫자나 문자가 섞여있습니다. 다시 입력하세요.");
			}

			for (TActorIterator<ABullsAndCowsPlayerController> It(GetWorld()); It; ++It)
			{
				ABullsAndCowsPlayerController* BullsAndCowsPlayerController = *It;
				if (IsValid(BullsAndCowsPlayerController) == true)
				{
					BullsAndCowsPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
				}
			}
		}
	}
}

void ABullsAndCowsGameModeBase::IncreaseGuessCount(ABullsAndCowsPlayerController* InChattingPlayerController)
{
	ABullsAndCowsPlayerState* BullsAndCowsPlayerState = InChattingPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
	if (IsValid(BullsAndCowsPlayerState) == true)
	{
		BullsAndCowsPlayerState->CurrentGuessCount++;
	}
}

void ABullsAndCowsGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& BullsAndCowsPlayerController : AllPlayerControllers)
	{
		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = BullsAndCowsPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
		if (IsValid(BullsAndCowsPlayerState) == true)
		{
			BullsAndCowsPlayerState->CurrentGuessCount = 0;
		}
	}
}

void ABullsAndCowsGameModeBase::JudgeGame(ABullsAndCowsPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = InChattingPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
		for (const auto& BullsAndCowsPlayerController : AllPlayerControllers)
		{
			if (IsValid(BullsAndCowsPlayerController) == true)
			{
				FString CombinedMessageString = BullsAndCowsPlayerState->PlayerNameString + TEXT(" has won the game");
				BullsAndCowsPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BullsAndCowsPlayerController : AllPlayerControllers)
		{
			ABullsAndCowsPlayerState* BullsAndCowsPlayerState = BullsAndCowsPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();

			if (IsValid(BullsAndCowsPlayerState) == true)
			{
				if (BullsAndCowsPlayerState->CurrentGuessCount < BullsAndCowsPlayerState->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
		if (true == bIsDraw)
		{
			for (const auto& BullsAndCowsPlayerController : AllPlayerControllers)
			{
				BullsAndCowsPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
				
				ResetGame();
			}
		}
	}
}

void ABullsAndCowsGameModeBase::OnMainTimerElapsed()
{
	ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
	if (IsValid(BullsAndCowsGameStateBase) == false)
	{
		return;
	}

	switch (BullsAndCowsGameStateBase->MatchState)
	{
	case EMatchState::None:
		break;
	case EMatchState::Waiting:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		if (AllPlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			NotificationString = FString::Printf(TEXT("Wait another players for playing."));

			RemainWaitingTimeForPlaying = WaitingTime; // 최소인원이 안된다면 대기 시간 초기화.
		}
		else
		{
			NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);

			--RemainWaitingTimeForPlaying;
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			NotificationString = FString::Printf(TEXT(""));

			BullsAndCowsGameStateBase->MatchState = EMatchState::Playing;
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Playing:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		NotificationString = FString::Printf(TEXT("남은 시간 : %d "), PlayTime);

		--PlayTime;

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Ending:
		break;
	case EMatchState::End:
		break;
	default:
		break;
	}
}

void ABullsAndCowsGameModeBase::NotifyToAllPlayer(const FString& NotificationString)
{
	for (auto GamePlayerController : AllPlayerControllers)
	{
		GamePlayerController->NotificationText = FText::FromString(NotificationString);
	}
}
