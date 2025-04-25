// CloudHu:604746493@qq.com All Rights Reserved


#include "Controllers/WarriorAIController.h"

#include "WarriorDebugHelper.h"
#include "Navigation/CrowdFollowingComponent.h"

AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	if (UCrowdFollowingComponent* CrowdFollowingComponent = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		Debug::Print(TEXT("CrowdFollowingComponent is valid."), FColor::Blue);
	}
}
