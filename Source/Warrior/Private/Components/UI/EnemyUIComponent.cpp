// CloudHu:604746493@qq.com All Rights Reserved


#include "Components/UI/EnemyUIComponent.h"

#include "Widgets/WarriorUserWidget.h"

void UEnemyUIComponent::RegisterEnemyDrawnWidget(UWarriorUserWidget* InWidget)
{
	EnemyDrawnWidgets.Add(InWidget);
}

void UEnemyUIComponent::RemoveDrawnWidgetIfAny()
{
	if (!EnemyDrawnWidgets.IsEmpty())
	{
		for (UWarriorUserWidget* DrawnWidget : EnemyDrawnWidgets)
		{
			if (DrawnWidget)
			{
				DrawnWidget->RemoveFromParent();
			}
		}
		EnemyDrawnWidgets.Empty();
	}
}
