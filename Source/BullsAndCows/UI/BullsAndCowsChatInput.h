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
	// 블루프린트에 해당 이름의 위젯 사용 강제
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
};
