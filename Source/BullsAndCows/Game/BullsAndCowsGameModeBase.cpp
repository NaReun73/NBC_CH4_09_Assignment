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
		BullsAndCowsPlayerController->NotificationText = FText::FromString(TEXT("게임 서버 접속."));

		AllPlayerControllers.Add(BullsAndCowsPlayerController);

		ABullsAndCowsPlayerState* BullsAndCowsPlayerState = BullsAndCowsPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
		if (IsValid(BullsAndCowsPlayerState) == true)
		{
			BullsAndCowsPlayerState->PlayerNameString = TEXT("Player ") + FString::FromInt(AllPlayerControllers.Num());
			PlayerTurnList.Add(BullsAndCowsPlayerState);
			UE_LOG(LogTemp, Log, TEXT("%s 등록 완료! 현재 총 인원: %d명"), *BullsAndCowsPlayerState->PlayerNameString, PlayerTurnList.Num());
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

	UE_LOG(LogTemp, Error, TEXT("%s"), *Result);

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

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.0f, true);

	RemainWaitingTimeForPlaying = WaitingTime;

	PlayTime = PlayTimeLimit;

}

void ABullsAndCowsGameModeBase::PrintChatMessageString(ABullsAndCowsPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	// 메시지 그대로 확인
	FString GuessNumberString = InChatMessageString;

	ABullsAndCowsPlayerState* BullsAndCowsPlayerState = InChattingPlayerController->GetPlayerState<ABullsAndCowsPlayerState>();
	if (IsValid(BullsAndCowsPlayerState) == true)
	{
		ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
		if (IsValid(BullsAndCowsGameStateBase) == true)
		{
			// 게임 상태가 Playing가 아니면 일반 채팅
			if (BullsAndCowsGameStateBase->MatchState != EMatchState::Playing)
			{
				FString CombinedMessageString = BullsAndCowsPlayerState->GetPlayerInfoString() + +TEXT(" : ") + InChatMessageString;

				for (TActorIterator<ABullsAndCowsPlayerController> It(GetWorld()); It; ++It)
				{
					ABullsAndCowsPlayerController* BullsAndCowsPlayerController = *It;
					if (IsValid(BullsAndCowsPlayerController) == true)
					{
						BullsAndCowsPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
					}
				}

				return;
			}
		}

		// 플레이어 턴 확인
		if (CheckPlayerTurn(BullsAndCowsPlayerState) == true)
		{
			// 정답 조건 확인
			if (IsGuessNumberString(GuessNumberString) == true && BullsAndCowsPlayerState->CurrentGuessCount < 3)
			{
				FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

				IncreaseGuessCount(InChattingPlayerController);

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

				EndTurn();
			}
			else
			{
				// 정답을 맞추려고 했지만 조건에 맞지않으면 일반 채팅으로
				FString CombinedMessageString = BullsAndCowsPlayerState->GetPlayerInfoString() + +TEXT(" : ") + InChatMessageString;

				// 어떤 조건인지 간단히 추가 출력
				if (GuessNumberString.Len() == 3)
				{
					if (BullsAndCowsPlayerState->CurrentGuessCount > 3)
					{
						CombinedMessageString = CombinedMessageString + TEXT(" (정답을 맞출 기회를 모두 사용했습니다.)");
					}
					else
					{
						CombinedMessageString = CombinedMessageString + TEXT(" (중복된 숫자나 문자가 섞여있습니다. 다시 입력하세요.");
					}
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
		else
		{
			//나의 턴이 아닐시 정답 확인을 할려해도 무시하고 일반 채팅
			FString CombinedMessageString = BullsAndCowsPlayerState->GetPlayerInfoString() + +TEXT(" : ") + InChatMessageString;

			if (InChatMessageString.Len() == 3)
			{
				CombinedMessageString = CombinedMessageString + TEXT(" (다른 플레이어의 턴입니다.)");
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
	ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
	if (IsValid(BullsAndCowsGameStateBase) == true)
	{
		BullsAndCowsGameStateBase->MatchState = EMatchState::Playing;
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
				FString CombinedMessageString = BullsAndCowsPlayerState->PlayerNameString + TEXT(" 의 승리 (잠시후 게임 재시작)");
				BullsAndCowsPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
				if (IsValid(BullsAndCowsGameStateBase) == true)
				{
					EndTime = EndTimeLimit;
					BullsAndCowsGameStateBase->MatchState = EMatchState::Ending;
				}
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
				BullsAndCowsPlayerController->NotificationText = FText::FromString(TEXT("무승부... (잠시후 게임 재시작)"));

				ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
				if (IsValid(BullsAndCowsGameStateBase) == true)
				{
					EndTime = EndTimeLimit;
					BullsAndCowsGameStateBase->MatchState = EMatchState::Ending;
				}
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
			NotificationString = FString::Printf(TEXT("다른 플레이어를 기다리는중."));

			RemainWaitingTimeForPlaying = WaitingTime; // 최소인원이 안된다면 대기 시간 초기화.
		}
		else
		{
			NotificationString = FString::Printf(TEXT("잠시후 게임이 시작됩니다 - %d초 ."), RemainWaitingTimeForPlaying);

			--RemainWaitingTimeForPlaying;
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			NotificationString = FString::Printf(TEXT(""));

			BullsAndCowsGameStateBase->MatchState = EMatchState::Playing;

			StartTurn(PlayerTurnList[0]);
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Playing:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		FString CurrentPlayerName = BullsAndCowsGameStateBase->CurrentTurnPlayerState->PlayerNameString;
		NotificationString = FString::Printf(TEXT("%s 의 턴 - 남은 시간 : %d "), *CurrentPlayerName, PlayTime);

		--PlayTime;

		if (PlayTime < 0)
		{
			if (PlayerTurnList[CurrentTurnIndex] == BullsAndCowsGameStateBase->CurrentTurnPlayerState)
			{
				PlayerTurnList[CurrentTurnIndex]->CurrentGuessCount++;
			}
			EndTurn();
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Ending:
	{
		--EndTime;

		if (EndTime < 0)
		{
			ResetGame();
		}

		break;
	}
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

void ABullsAndCowsGameModeBase::StartTurn(ABullsAndCowsPlayerState* NewPlayerState)
{
	ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
	if (IsValid(BullsAndCowsGameStateBase) == true && NewPlayerState)
	{
		BullsAndCowsGameStateBase->CurrentTurnPlayerState = NewPlayerState;

		PlayTime = PlayTimeLimit;

		UE_LOG(LogTemp, Log, TEXT("현재 플레이어 : %s"), *NewPlayerState->PlayerNameString);
	}
}

void ABullsAndCowsGameModeBase::EndTurn()
{
	ChooseNextPlayer();
}

void ABullsAndCowsGameModeBase::ChooseNextPlayer()
{
	if (PlayerTurnList.Num() <= 1)
	{
		return;
	}

	CurrentTurnIndex = (CurrentTurnIndex + 1) % PlayerTurnList.Num();

	PlayTime = PlayTimeLimit;

	StartTurn(PlayerTurnList[CurrentTurnIndex]);
}

bool ABullsAndCowsGameModeBase::CheckPlayerTurn(ABullsAndCowsPlayerState* RequestingPlayer)
{
	ABullsAndCowsGameStateBase* BullsAndCowsGameStateBase = GetGameState<ABullsAndCowsGameStateBase>();
	if (IsValid(BullsAndCowsGameStateBase) == true)
	{
		return BullsAndCowsGameStateBase->CurrentTurnPlayerState == RequestingPlayer;
	}

	return false;
}

