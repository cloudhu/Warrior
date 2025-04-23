// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorGameplayAbility.generated.h"

class UWarriorAbilitySystemComponent;
class UPawnCombatComponent;

UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy :uint8
{
	OnTriggered,
	OnGiven
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category=WarriorAbility)
	EWarriorAbilityActivationPolicy AbilityActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;

	//~ Begin UGameplayAbility Interface.
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface.

	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category="Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;

	FActiveGameplayEffectHandle NativeApplyGameplayEffectSpecHandle(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category="Warrior|Ability", meta=(DisplayName="Apply Gameplay EffectSpec Handle To Target Actor", ExpandEnumAsExecs="OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyGameplaySpecEffectHandleToTargetActor(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle,
	                                                                          EWarriorSuccessType& OutSuccessType);
};
