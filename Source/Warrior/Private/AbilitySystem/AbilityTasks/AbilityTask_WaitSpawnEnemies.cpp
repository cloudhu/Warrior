// CloudHu:604746493@qq.com All Rights Reserved


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Characters/MonsterCharacter.h"
#include "Engine/AssetManager.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitAndSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag,
                                                                                  TSoftClassPtr<AMonsterCharacter> SoftEnemyClassToSpawn, const int32 NumToSpawn,
                                                                                  const FVector& SpawnOrigin, const float RandomSpawnRadius)
{
	UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedSpawnOrigin = SpawnOrigin;

	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	return Node;
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
		                                                             FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded));
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AMonsterCharacter*>());
		}
		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();
	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AMonsterCharacter*>());
		}
		EndTask();
		return;
	}

	TArray<AMonsterCharacter*> SpawnedEnemies;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);

		RandomLocation += FVector(0.f, 0.f, 150.f);
		const FRotator SpawnRotation=AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();

		if (AMonsterCharacter* MonsterCharacter = World->SpawnActor<AMonsterCharacter>(LoadedClass, RandomLocation, SpawnRotation, SpawnParameters))
		{
			SpawnedEnemies.Add(MonsterCharacter);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (SpawnedEnemies.IsEmpty())
		{
			DidNotSpawn.Broadcast(TArray<AMonsterCharacter*>());
		}
		else
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
	}

	EndTask();
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	// Super::Activate();
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);
	Super::OnDestroy(bInOwnerFinished);
}
