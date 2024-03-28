// Fill out your copyright notice in the Description page of Project Settings.


#include "PullPushObject.h"
#include "Character/MainCharacter.h"

// Sets default values
APullPushObject::APullPushObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ObjectMesh);
	LocalHandedPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LocalPoint"));
	LocalHandedPoint->SetupAttachment(ObjectMesh);
}

// Called when the game starts or when spawned
void APullPushObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void APullPushObject::Interaction(AActor* character)
{
	auto charact = Cast<AMainCharacter>(character);
	if (charact) {
		charact->PullPush(this);
	}
}

// Called every frame
void APullPushObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

