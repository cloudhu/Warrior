// CloudHu:604746493@qq.com All Rights Reserved


#include "WarriorFunctionLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarriorTypes/WarriorCountDownAction.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::InValid;
	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(const APawn* QueryPawn, const APawn* TargetPawn)
{
	if (QueryPawn && TargetPawn)
	{
		if (const IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController()))
		{
			// Debug::Print(TEXT("QueryTeamAgent GetGenericTeamId:%s"), QueryTeamAgent->GetGenericTeamId());
			if (const IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController()))
			{
				return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
			}
		}
	}
	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(const AActor* InAttacker, const AActor* InVictim, float& OutAngle)
{
	check(InAttacker&&InVictim);
	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngle = UKismetMathLibrary::DegAcos(DotResult);

	if (const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized); CrossResult.Z < 0.f)
	{
		OutAngle *= -1.f;
	}

	if (OutAngle >= -45.f && OutAngle <= 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngle < -45.f && OutAngle >= -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngle < -135.f || OutAngle > 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	return WarriorGameplayTags::Shared_Status_HitReact_Right;
}

bool UWarriorFunctionLibrary::IsValidBlock(const AActor* InAttacker, const AActor* InDefender)
{
	check(InAttacker&&InDefender);
	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());
	return DotResult < -0.1f;
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
                                                                         const FGameplayEffectSpecHandle InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceAsc = NativeGetASCFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetAsc = NativeGetASCFromActor(InTargetActor);
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceAsc->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetAsc);
	Debug::Print(TEXT("WasSuccessfullyApplied") + ActiveGameplayEffectHandle.WasSuccessfullyApplied());
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime,
                                        EWarriorCountDownActionInput CountDownInput,UPARAM(DisplayName=Output)
                                        EWarriorCountDownActionOutput&  CountDownOutput, FLatentActionInfo LatentInfo)
{
	if (GEngine)
	{
		if (UWorld* World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::ReturnNull))
		{
			FLatentActionManager& LatentActionManager=World->GetLatentActionManager();
			FWarriorCountDownAction* FoundAction=LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget,LatentInfo.UUID);
			if (CountDownInput == EWarriorCountDownActionInput::Start)
			{
				if (!FoundAction)
				{
					LatentActionManager.AddNewAction(
						LatentInfo.CallbackTarget,
						LatentInfo.UUID,
						new FWarriorCountDownAction(TotalTime,UpdateInterval,OutRemainingTime,CountDownOutput,LatentInfo)
					);
				}
			}

			if (CountDownInput == EWarriorCountDownActionInput::Cancel)
			{
				if (FoundAction)
				{
					FoundAction->CancelAction();
				}
			}
		}
	}
}
