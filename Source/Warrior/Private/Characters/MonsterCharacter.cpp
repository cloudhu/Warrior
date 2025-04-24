// CloudHu:604746493@qq.com All Rights Reserved


#include "Characters/MonsterCharacter.h"

#include "Components/Combat/EnemyCombatComponent.h"
#include "Components/UI/EnemyUIComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"

AMonsterCharacter::AMonsterCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));
}

UPawnCombatComponent* AMonsterCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

void AMonsterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
}

UPawnUIComponent* AMonsterCharacter::GetPawnUIComponent() const
{
	// return Super::GetPawnUIComponent();
	return EnemyUIComponent;
}

void AMonsterCharacter::InitEnemyStartUpData() const
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(CharacterStartUpData.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]()
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}));
}
