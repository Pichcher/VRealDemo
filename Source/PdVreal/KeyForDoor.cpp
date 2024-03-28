// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyForDoor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Character/MainCharacter.h"

// Sets default values
AKeyForDoor::AKeyForDoor() :
	PickedUp(false),
	CanTake(false)
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot")); // create DefaultSceneRoot
	RootComponent = DefaultSceneRoot;

	key_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh")); // create key
	key_mesh->SetupAttachment(RootComponent);

	CollisionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionZone")); // create collisionBox
	CollisionZone->SetupAttachment(RootComponent);
	CollisionZone->SetHiddenInGame(true);

	CollisionZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionZone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

	// func Begin and End Overlap
	CollisionZone->OnComponentBeginOverlap.AddDynamic(this, &AKeyForDoor::OnOverlapZone);
	CollisionZone->OnComponentEndOverlap.AddDynamic(this, &AKeyForDoor::OutOverlapZone);

}

// Called when the game starts or when spawned
void AKeyForDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKeyForDoor::FuncInteract(AActor* character)
{
	AKeyForDoor::Interaction(character);
}


void AKeyForDoor::Interaction(AActor* character)
{
	if (!PickedUp && CanTake)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Key picked up"));//Notification
		PickedUp = true;

		AMainCharacter* charact = Cast<AMainCharacter>(character);
		charact->CharacterStop(true);//disable movement

		/*There should be animation here*/
		charact->PlayAnimTakeKey();

		GetWorldTimerManager().SetTimer(UnusedHandle, [this, charact] {AKeyForDoor::Attachment(charact); }, 0.75f, false);// timer after which we attach
	}
}
//function for attach
void AKeyForDoor::Attachment(AMainCharacter* character)
{
	key_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//disable collision 
	key_mesh->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("ring_01_r"));// attach key to right hand

	if (SoundToPickUp)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPickUp);
	}

	character->SetGotKey(true); //changing the character variable
	character->SetKey(key_mesh);// Set ref for key component

	//character->CharacterStop(false);//turn on movement
}

void AKeyForDoor::OnOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto character = Cast<AMainCharacter>(OtherActor);
	if (character) {
		CanTake = true;
		character->SetActorToInteract(this);
	}
}

void AKeyForDoor::OutOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto character = Cast<AMainCharacter>(OtherActor);
	if (character) {
		CanTake = false;
		character->SetActorToInteract(nullptr);
	}
}

// Called every frame
void AKeyForDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

