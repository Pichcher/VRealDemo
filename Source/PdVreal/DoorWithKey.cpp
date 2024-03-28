// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorWithKey.h"
#include "Components/BoxComponent.h"
#include "Character/MainCharacter.h"

// Sets default values
ADoorWithKey::ADoorWithKey() :
	CanOpen(false),
	DoorOpen(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot")); //create DefaultSceneRoot
	RootComponent = DefaultSceneRoot;

	key_reader = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("key_reader")); // create key_reader
	key_reader->SetupAttachment(RootComponent);

	CollisionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionZone")); // create collisionBox
	CollisionZone->SetupAttachment(RootComponent);
	CollisionZone->SetHiddenInGame(true);

	interact_point = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("interact_point")); // create interact_point
	interact_point->SetupAttachment(RootComponent);
	interact_point->SetHiddenInGame(true);

	CollisionZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionZone->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionZone->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionZone->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

	// func Begin and End Overlap
	CollisionZone->OnComponentBeginOverlap.AddDynamic(this, &ADoorWithKey::OnOverlapZone);
	CollisionZone->OnComponentEndOverlap.AddDynamic(this, &ADoorWithKey::OutOverlapZone);

}

// Called when the game starts or when spawned
void ADoorWithKey::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoorWithKey::FuncInteract(AActor* character)
{
	ADoorWithKey::Interaction(character);
}

void ADoorWithKey::Interaction(AActor* character)
{
	/*if (!DoorOpen && CanOpen)
	{
		AMainCharacter* charact = Cast<AMainCharacter>(character);
		if (charact->GetGotKey()) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Door open"));//Notification
			DoorOpen = true;

			charact->CharacterStop(true);//disable movement


			GetWorldTimerManager().SetTimer(UnusedHandle, [this, charact] {ADoorWithKey::AttachKeyToDoor(charact); }, 2.f, false);// timer after which we attach
		}
	}*/
}
void ADoorWithKey::AttachKeyToDoor(AMainCharacter* character)
{
	key = character->GetKey();
	key->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // detach for right hand
	key->AttachToComponent(key_reader, FAttachmentTransformRules::SnapToTargetNotIncludingScale);// attach key to key_reader
	key->SetRelativeRotation(FRotator(0.f,0.f,90.f));
	OpeningDoor(); // open door with timeline on BP
}

void ADoorWithKey::OnOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto character = Cast<AMainCharacter>(OtherActor);
	if (character) {
		CanOpen = true;
		if (!DoorOpen && CanOpen)
		{
			AMainCharacter* charact = Cast<AMainCharacter>(character);
			if (charact->GetGotKey())
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Door open"));//Notification
				DoorOpen = true;

				charact->CharacterStop(true);//disable movement

				charact->PlayAnimUseKey(interact_point->GetComponentTransform());

				GetWorldTimerManager().SetTimer(UnusedHandle, [this, charact] {ADoorWithKey::AttachKeyToDoor(charact); }, 1.8f, false);// timer after which we attach
			}
		}
		character->SetActorToInteract(this);
	}
}

void ADoorWithKey::OutOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto character = Cast<AMainCharacter>(OtherActor);
	if (character) {
		CanOpen = false;
		//character->SetActorToInteract(nullptr);
	}
}

// Called every frame
void ADoorWithKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

