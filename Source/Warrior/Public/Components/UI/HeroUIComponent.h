// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdateDelegate, FGameplayTag, AbilityInputTag, TSoftObjectPtr<UMaterialInterface>,
                                             SoftAbilityIconMaterial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBegin, FGameplayTag, AbilityInputTag, float, TotalCooldownTime, float, RemainingCooldownTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoneInteractedDelegate,bool,bShouldDisplayInputKey);
/**
 * UHeroUIComponent is a UI component class specific to hero characters, extending UPawnUIComponent.
 * It provides delegates for various UI-related events that can be assigned and triggered within Blueprints.
 * This component facilitates communication between gameplay logic and the UI layer, enabling dynamic updates
 * to the hero's UI elements such as rage, equipped weapon, and ability icons.
 *
 * Delegates:
 * - OnCurrentRageChanged: Triggered when the hero's current rage percentage changes. Useful for updating rage bars or indicators.
 * - OnEquippedWeaponChanged: Triggered when the hero's equipped weapon changes. Allows UI updates for weapon-specific visuals or stats.
 * - OnAbilityIconSlotUpdateDelegate: Triggered when there is an update to the ability icon slots. Used to refresh ability-related UI elements.
 *
 * This component is typically retrieved via the IPawnUIInterface and is integrated into the UI workflow through events like BP_OnOwningHeroUIComponentInitialized.
 * It plays a key role in synchronizing hero attributes and states with the corresponding visual representation in the UI.
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangeDelegate OnCurrentRageChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityIconSlotUpdateDelegate OnAbilityIconSlotUpdateDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityCooldownBegin OnAbilityCooldownBegin;

	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnStoneInteractedDelegate OnStoneInteracted;
};
