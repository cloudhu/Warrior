// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AttributeSet.h"
#include "WarriorAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * @class UWarriorAttributeSet
 * @brief A class that extends UAttributeSet to manage specific attributes for a warrior character, such as health and rage.
 *
 * This class is used within the gameplay ability system to track and modify the current and maximum health, as well as the current rage of a warrior. It provides accessors for these attributes, allowing them to be read and modified in a blueprint or C++ context.
 */
UCLASS()
class WARRIOR_API UWarriorAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UWarriorAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category=Health)
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentHealth)

	UPROPERTY(BlueprintReadOnly, Category=Health)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxHealth)


	/**
	 * 怒气
	 */
	UPROPERTY(BlueprintReadOnly, Category=Rage)
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentRage)

	UPROPERTY(BlueprintReadOnly, Category=Rage)
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxRage)

	UPROPERTY(BlueprintReadOnly, Category=Damage)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category=Damage)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DefensePower)

	UPROPERTY(BlueprintReadOnly, Category=Damage)
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DamageTaken)

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
