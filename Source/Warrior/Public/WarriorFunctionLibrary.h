// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorFunctionLibrary.generated.h"

class UWarriorGameInstance;
struct FScalableFloat;
class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;

/**
 * A static function library containing utility functions for the Warrior game framework.
 * This class provides a collection of helper methods for gameplay-related operations,
 * including actor manipulation, gameplay tag management, combat component access, and more.
 */
UCLASS()
class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static UWarriorAbilitySystemComponent* NativeGetASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary", meta=(DisplayName="Does Actor Have Tag", ExpandEnumAsExecs="OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary", meta=(DisplayName="Get Pawn Combat Component From Actor", ExpandEnumAsExecs="OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary")
	static bool IsTargetPawnHostile(const APawn* QueryPawn, const APawn* TargetPawn);

	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary", meta=(CompactNodeTitle="Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.f);

	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(const AActor* InAttacker, const AActor* InVictim, float& OutAngle);

	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary")
	static bool IsValidBlock(const AActor* InAttacker, const AActor* InDefender);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle InSpecHandle);

	UFUNCTION(BlueprintCallable, Category="Warrior|FunctionLibrary", meta=(Latent, WorldContext=WorldContextObject, LatentInfo=LatentInfo,
		ExpandEnumAsExecs="CountDownInput|CountDownOutput", TotalTime=1, UpdateInterval="0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime, EWarriorCountDownActionInput
	                      CountDownInput, UPARAM(DisplayName=Output) EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintPure, Category="Warrior|FunctionLibrary",meta=(WorldContext=WorldContextObject))
	static UWarriorGameInstance* GetWarriorGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static void ToggleInputMode(const UObject* WorldContextObject,EWarriorInputMode InInputMode);

	/**
	 * Saves the current game difficulty to the save game slot.
	 *
	 * This function creates a save game object of type UWarriorSaveGame and assigns the provided difficulty level to it.
	 * The save game object is then saved to a predefined slot using the UGameplayStatics::SaveGameToSlot function.
	 *
	 * @param InDifficultyToSave The game difficulty level to be saved.
	 *                           Must be one of the values from the EWarriorGameDifficulty enumeration.
	 */
	UFUNCTION(BlueprintCallable,Category = "Warrior|FunctionLibrary")
	static void SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave);

	/**
	 * Attempts to load the saved game difficulty from a predefined save slot.
	 *
	 * This function checks if a save game exists in the specified slot. If it does, the save game is loaded
	 * and cast to UWarriorSaveGame. If the cast is successful, the saved game difficulty is retrieved and
	 * assigned to the OutSavedDifficulty parameter. If the save game does not exist or the cast fails,
	 * the function returns false.
	 *
	 * @param OutSavedDifficulty Reference to the variable where the loaded game difficulty will be stored.
	 *                            Must be of type EWarriorGameDifficulty.
	 * @return True if the saved game difficulty was successfully loaded, false otherwise.
	 */
	UFUNCTION(BlueprintCallable,Category = "Warrior|FunctionLibrary")
	static bool TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSavedDifficulty);
};
