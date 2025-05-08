// CloudHu:604746493@qq.com All Rights Reserved


#include "Items/PickUps/WarriorPickUpBase.h"

#include "Components/SphereComponent.h"

// Sets default values
AWarriorPickUpBase::AWarriorPickUpBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
	PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionSphereBeginOverlap);
}

void AWarriorPickUpBase::OnCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                                                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
}
