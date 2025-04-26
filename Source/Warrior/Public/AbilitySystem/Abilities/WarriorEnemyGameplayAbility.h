// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorEnemyGameplayAbility.generated.h"

class UEnemyCombatComponent;
class AMonsterCharacter;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	AMonsterCharacter* GetMonsterCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(const TSubclassOf<UGameplayEffect>& EffectClass, const FScalableFloat& InDamageScalableFloat) const;
private:
	TWeakObjectPtr<AMonsterCharacter> CachedMonsterCharacter;
};
