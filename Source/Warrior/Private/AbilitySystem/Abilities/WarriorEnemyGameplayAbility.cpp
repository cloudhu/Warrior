// CloudHu:604746493@qq.com All Rights Reserved


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"

#include "Characters/MonsterCharacter.h"

AMonsterCharacter* UWarriorEnemyGameplayAbility::GetMonsterCharacterFromActorInfo()
{
	if (!CachedMonsterCharacter.IsValid())
	{
		CachedMonsterCharacter = Cast<AMonsterCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedMonsterCharacter.IsValid() ? CachedMonsterCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetMonsterCharacterFromActorInfo()->GetEnemyCombatComponent();
}
