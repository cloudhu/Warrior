// CloudHu:604746493@qq.com All Rights Reserved


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Characters/MonsterCharacter.h"
#include "Components/BoxComponent.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}
	OverlappedActors.AddUnique(HitActor);

	//block check
	bool bIsValidBlock = false;
	const bool bIsMyAttackUnblockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), WarriorGameplayTags::Enemy_Status_Unblockable);
	if (const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WarriorGameplayTags::Player_Status_Blocking); bIsPlayerBlocking && !
		bIsMyAttackUnblockable)
	{
		//check if the block is valid
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;

	if (bIsValidBlock)
	{
		//Handle successful block
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, WarriorGameplayTags::Player_Event_SuccessfulBlock, EventData);
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

void UEnemyCombatComponent::ToggleBodyCollisionBox(const bool bShouldEnable, const EToggleDamageType ToggleDamageType)
{
	// Super::ToggleBodyCollisionBox(bShouldEnable, ToggleDamageType);
	const AMonsterCharacter* OwningEnemyCharacter = GetOwningPawn<AMonsterCharacter>();
	check(OwningEnemyCharacter);

	UBoxComponent* LeftHand = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHand = OwningEnemyCharacter->GetRightHandCollisionBox();

	check(LeftHand && RightHand);

	switch (ToggleDamageType)
	{
	case EToggleDamageType::CurrentEquippedWeapon:
		break;
	case EToggleDamageType::LeftHand:
		LeftHand->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision);
		break;
	case EToggleDamageType::RightHand:
		RightHand->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision);
		break;
	}

	if (!bShouldEnable && !OverlappedActors.IsEmpty())
	{
		OverlappedActors.Empty();
	}
}
