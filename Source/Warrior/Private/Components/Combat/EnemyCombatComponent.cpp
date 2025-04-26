// CloudHu:604746493@qq.com All Rights Reserved


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGameplayTags.h"
#include "Abilities/GameplayAbilityTypes.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	OverlappedActors.AddUnique(HitActor);

	//block check
	bool bIsValidBlock = false;
	constexpr bool bIsMyAttackUnblockable = false;
	if (constexpr bool bIsPlayerBlocking = false; bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		//check if the block is valid
		bIsValidBlock = true;
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;

	if (bIsValidBlock)
	{
		//Handle successful block
	}
	else
	{
		//block failed,Send Gameplay Event
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), WarriorGameplayTags::Shared_Event_MeleeHit, EventData);
	}
}

void UEnemyCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}
