// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"

class UWarriorUserWidget;
/**
 * 
 */
UCLASS()
class WARRIOR_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawnWidget(UWarriorUserWidget* InWidget);

	UFUNCTION(BlueprintCallable)
	void RemoveDrawnWidgetIfAny();
	
private:
	TArray<UWarriorUserWidget*> EnemyDrawnWidgets;
};
