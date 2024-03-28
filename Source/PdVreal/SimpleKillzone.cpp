// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleKillzone.h"
#include "Components/BoxComponent.h"
#include "Character/MainCharacter.h"
// Sets default values
ASimpleKillzone::ASimpleKillzone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionZone"));
	SetRootComponent(CollisionZone);
	CollisionZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionZone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CollisionZone->OnComponentBeginOverlap.AddDynamic(this, &ASimpleKillzone::OnOverlapZone);
	
}

// Called when the game starts or when spawned
void ASimpleKillzone::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASimpleKillzone::OnOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto character = Cast<AMainCharacter>(OtherActor);
	if (character) {
		character->CharacterDied();
	}
}

// Called every frame
void ASimpleKillzone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

