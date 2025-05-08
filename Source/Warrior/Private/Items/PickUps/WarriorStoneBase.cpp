// CloudHu:604746493@qq.com All Rights Reserved


#include "Items/PickUps/WarriorStoneBase.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, const int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);

	const UGameplayEffect* EffectCDO = StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();

	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		EffectCDO,
		ApplyLevel,
		AbilitySystemComponent->MakeEffectContext()
	);

	BP_OnStoneConsumed();
}

void AWarriorStoneBase::OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                                                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	Super::OnCollisionSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepHitResult);

	if (const AWarriorHeroCharacter* HeroCharacter = Cast<AWarriorHeroCharacter>(OtherActor))
	{
		HeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGameplayTags::Player_Ability_PickUp_Stones);
	}
}
