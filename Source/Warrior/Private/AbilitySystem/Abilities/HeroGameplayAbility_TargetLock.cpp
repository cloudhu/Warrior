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

/**
 * Updates the target lock behavior during each tick while the ability is active.
 *
 * This method performs several checks and actions to maintain the target lock functionality:
 * - Retrieves the hero character using `GetHeroCharacterFromActorInfo`. If no valid hero character is found, the target lock ability is canceled.
 * - Verifies if the current locked actor or the hero character has the "dead" status tag (`WarriorGameplayTags::Shared_Status_Dead`).
 *   If either is dead, the target lock ability is canceled.
 * - Calls `SetTargetLockWidgetPosition` to update the position of the target lock UI widget.
 * - Determines whether the hero character's rotation should be overridden based on the absence of specific tags:
 *   - The hero character must not have the "rolling" tag (`WarriorGameplayTags::Player_Status_Rolling`).
 *   - The hero character must not have the "blocking" tag (`WarriorGameplayTags::Player_Status_Blocking`).
 * - If rotation override is allowed, calculates the desired look-at rotation toward the locked actor using `UKismetMathLibrary::FindLookAtRotation`.
 *   Adjusts the rotation by subtracting a camera offset distance (`TargetLockCameraOffsetDistance`) along the pitch axis.
 * - Interpolates between the current control rotation and the calculated target rotation using `FMath::RInterpTo` with a specified interpolation speed (`TargetLockRotationInterpSpeed`).
 * - Updates the hero controller's control rotation and the hero character's actor rotation to align with the interpolated target rotation.
 *
 * @param DeltaTime The time elapsed since the last frame, used for smooth interpolation of the rotation.
 *
 * @see GetHeroCharacterFromActorInfo
 * @see CancelTargetLockAbility
 * @see SetTargetLockWidgetPosition
 * @see NativeDoesActorHaveTag
 * @see UKismetMathLibrary::FindLookAtRotation
 * @see FMath::RInterpTo
 */
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
			FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(HeroCharacter->GetActorLocation(), CurrentLockedActor->GetActorLocation());
			LookAtRotator -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);

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

/**
 * Switches the currently locked target to a new target based on the specified switch direction.
 *
 * This method first retrieves the list of available actors for target locking by calling `GetAvailableActorsToLock`.
 * It then determines the actors located to the left and right of the current target by invoking `GetAvailableActorsAroundTarget`.
 * Depending on the provided switch direction tag, it selects the nearest actor from either the left or right side
 * using `GetNearestTargetFromAvailableActors`. If a valid target is found, it updates the `CurrentLockedActor` property.
 *
 * The switch direction is determined by comparing the `InSwitchDirectionTag` parameter with predefined gameplay tags.
 * Specifically, `WarriorGameplayTags::Player_Event_SwitchTarget_Left` indicates switching to the left, while any other tag
 * switches to the right.
 *
 * @param InSwitchDirectionTag The gameplay tag specifying the direction to switch the target.
 *                              Expected values are `WarriorGameplayTags::Player_Event_SwitchTarget_Left` for switching left
 *                              or any other tag for switching right.
 *
 * @see GetAvailableActorsToLock
 * @see GetAvailableActorsAroundTarget
 * @see GetNearestTargetFromAvailableActors
 * @see CurrentLockedActor
 */
void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;

	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);

	if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
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

/**
 * Performs a box trace to determine the list of actors available for target locking.
 * This method clears the existing list of available actors and performs a multi-object box trace
 * in the forward direction of the hero character. The trace results are filtered to exclude the
 * hero character itself, and unique actors are added to the list of available actors for locking.
 *
 * The trace parameters, such as distance, box size, and object channels, are configurable via
 * class properties. Debug visualization of the trace can be enabled or disabled based on the
 * `bShowPersistentDebugShape` property.
 *
 * @see AvailableActorsToLock
 * @see TraceDistance
 * @see TraceBoxSize
 * @see BoxTraceChannel
 * @see bShowPersistentDebugShape
 */
void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
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

/**
 * Retrieves the available actors around the currently locked target and categorizes them into two groups:
 * actors on the left and actors on the right, relative to the player's perspective.
 *
 * This method first checks if there is a currently locked actor and if the list of available actors for locking
 * is not empty. If either condition is not met, the target lock ability is canceled.
 *
 * The method calculates the normalized direction vector from the player's location to the currently locked actor.
 * It then iterates through the list of available actors, excluding the currently locked actor, and computes the
 * normalized direction vector from the player's location to each available actor.
 *
 * Using the cross product of the two normalized vectors, the method determines whether each available actor is
 * positioned to the left or right of the currently locked actor from the player's perspective. Actors are added
 * to the respective output arrays based on this determination.
 *
 * @param OutActorsOnLeft  An array that will be populated with actors positioned to the left of the currently locked actor.
 * @param OutActorsOnRight An array that will be populated with actors positioned to the right of the currently locked actor.
 *
 * @see AvailableActorsToLock
 * @see CurrentLockedActor
 * @see CancelTargetLockAbility
 */
void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor == CurrentLockedActor)continue;

		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		if (const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized); CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
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
