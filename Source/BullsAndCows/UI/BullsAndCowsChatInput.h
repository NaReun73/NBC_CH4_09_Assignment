#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BullsAndCowsChatInput.generated.h"

class UEditableTextBox;

/**
 * 
 */
UCLASS()
class BULLSANDCOWS_API UBullsAndCowsChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
public:
	// 이 클래스를 상속받아 위젯 블루프린트를 만들시 EditableTextBox_ChatInput이란 이름을 가지고 있는 EditableTextBox을 강제함
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
};
