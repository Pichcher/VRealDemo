// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyCard.h"
#include "Components/SphereComponent.h"
#include "Character/MainCharacter.h"

// Sets default values
AKeyCard::AKeyCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	KeyCardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyCardMesh"));
	SetRootComponent(KeyCardMesh);
	CollisionZone = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionZone"));
	CollisionZone->SetupAttachment(RootComponent);
	CollisionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

// Called when the game starts or when spawned
void AKeyCard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKeyCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKeyCard::Interaction(AActor* character)
{
}

