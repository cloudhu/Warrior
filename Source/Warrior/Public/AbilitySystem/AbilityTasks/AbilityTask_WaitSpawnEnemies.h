// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

class AMonsterCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitAndSpawnEnemiesDelegate, const TArray<AMonsterCharacter*>&, SpawnEnemies);

/**
 * 
 */
UCLASS()
class WARRIOR_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Warrior|AbilityTasks",
		meta=(DisplayName="Wait Gameplay Event And Spawn Enemies", HidePin=OwningAbility, DefaultToSelf=OwningAbility, BlueprintInternalUseOnly=true, NumToSpawn=1,
			RandomSpawnRadius=200 ))
	static UAbilityTask_WaitSpawnEnemies* WaitAndSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag, TSoftClassPtr<AMonsterCharacter>
	                                                          SoftEnemyClassToSpawn, const int32 NumToSpawn, const FVector& SpawnOrigin, const float RandomSpawnRadius);

	UPROPERTY(BlueprintAssignable)
	FWaitAndSpawnEnemiesDelegate OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FWaitAndSpawnEnemiesDelegate DidNotSpawn;

private:
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AMonsterCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;

	FDelegateHandle DelegateHandle;

	void OnGameplayEventReceived(const FGameplayEventData* InPayload);

	void OnEnemyClassLoaded();
protected:
	virtual void Activate() override;

public:
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
