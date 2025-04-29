// CloudHu:604746493@qq.com All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

#include "EnhancedInputSubsystems.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/SizeBox.h"
#include "Controllers/WarriorHeroController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/WarriorUserWidget.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUp();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (AWarriorHeroCharacter* HeroCharacter = GetHeroCharacterFromActorInfo())
	{
		if (!CurrentLockedActor || UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, WarriorGameplayTags::Shared_Status_Dead)
			|| UWarriorFunctionLibrary::NativeDoesActorHaveTag(HeroCharacter, WarriorGameplayTags::Shared_Status_Dead))
		{
			CancelTargetLockAbility();
			return;
		}

		SetTargetLockWidgetPosition();

		const bool bShouldOverrideRotation = !UWarriorFunctionLibrary::NativeDoesActorHaveTag(HeroCharacter, WarriorGameplayTags::Player_Status_Rolling)
			&& !UWarriorFunctionLibrary::NativeDoesActorHaveTag(HeroCharacter, WarriorGameplayTags::Player_Status_Blocking);
		if (bShouldOverrideRotation)
		{
			const FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(HeroCharacter->GetActorLocation(), CurrentLockedActor->GetActorLocation());
			const FRotator CurrentControlRotator = GetHeroControllerFromActorInfo()->GetControlRotation();
			const FRotator TargetRotator = FMath::RInterpTo(CurrentControlRotator, LookAtRotator, DeltaTime, TargetLockRotationInterpSpeed);

			GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRotator.Pitch, TargetRotator.Yaw, 0.f));
			HeroCharacter->SetActorRotation(FRotator(0.f, TargetRotator.Yaw, 0.f));
		}
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed = GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	if (const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		checkf(Subsystem, TEXT("Subsystem is invalid,please check C++ code at function : UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext"));

		Subsystem->AddMappingContext(TargetLockMappingContext, 3);
	}
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetHeroControllerFromActorInfo())return;
	if (const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		checkf(Subsystem, TEXT("Subsystem is invalid,please check C++ code at function : UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext"));

		Subsystem->RemoveMappingContext(TargetLockMappingContext);
	}
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();

	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock);

	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	TArray<FHitResult> BoxTraceHits;
	const AWarriorHeroCharacter* HeroCharacter = GetHeroCharacterFromActorInfo();
	UKismetSystemLibrary::BoxTraceMultiForObjects(HeroCharacter, HeroCharacter->GetActorLocation(),
	                                              HeroCharacter->GetActorLocation() + HeroCharacter->GetActorForwardVector() * TraceDistance, TraceBoxSize / 2.f,
	                                              HeroCharacter->GetActorForwardVector().ToOrientationRotator(), BoxTraceChannel, false, TArray<AActor*>(),
	                                              bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None, BoxTraceHits, true);

	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != HeroCharacter)
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (DrawnTargetLockWidget)
	{
		return;
	}
	checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in Blueprint.UHeroGameplayAbility_TargetLock::DrawTargetLockWidget"));

	DrawnTargetLockWidget = CreateWidget<UWarriorUserWidget>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);
	check(DrawnTargetLockWidget);

	DrawnTargetLockWidget->AddToViewport();
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetHeroControllerFromActorInfo(), CurrentLockedActor->GetActorLocation(), ScreenPosition, true);

	if (TargetLockWidgetSize == FVector2D::Zero())
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget([this](UWidget* FoundWidget)
		{
			if (const USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
			{
				TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
				TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
			}
		});
	}

	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;

	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed = 0.f;
}
