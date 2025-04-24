// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "MonsterCharacter.generated.h"

class UEnemyUIComponent;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AMonsterCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	AMonsterCharacter();
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat)
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=UI)
	UEnemyUIComponent* EnemyUIComponent;

public:
	virtual UPawnUIComponent* GetPawnUIComponent() const override;

private:
	void InitEnemyStartUpData() const;
public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};
