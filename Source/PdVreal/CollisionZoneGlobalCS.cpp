// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionZoneGlobalCS.h"
#include "Components/BoxComponent.h"
#include "Character/MainCharacter.h"

// Sets default values
ACollisionZoneGlobalCS::ACollisionZoneGlobalCS()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
}

// Called when the game starts or when spawned
void ACollisionZoneGlobalCS::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACollisionZoneGlobalCS::OnOverlapZone);
}

void ACollisionZoneGlobalCS::OnOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AMainCharacter>(OtherActor);
	if (Character) {
		Character->ChangeToWorldCameraTranform(MustTransformIn);
	}
}

// Called every frame
void ACollisionZoneGlobalCS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

