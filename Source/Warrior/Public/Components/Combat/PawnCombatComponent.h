// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

UENUM(BlueprintType)
enum class EToggleDamageType:uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTag, AWarriorWeaponBase* InWeapon, bool bRegisteredAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category="Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
	void ToggleWeaponCollision(const bool bShouldEnable,const EToggleDamageType ToggleDamageType=EToggleDamageType::CurrentEquippedWeapon);

	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);

protected:
	virtual void ToggleCurrentEquippedWeaponCollision(const bool bShouldEnable);
	
	virtual void ToggleBodyCollisionBox(const bool bShouldEnable,const EToggleDamageType ToggleDamageType);
	
	TArray<AActor*> OverlappedActors;

private:
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;
};
