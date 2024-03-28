// Fill out your copyright notice in the Description page of Project Settings.


#include "SImpleButton.h"
#include "Character/MainCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
ASImpleButton::ASImpleButton() :
	pressed_time(0.f),
	is_pressed(false)
{	
	PrimaryActorTick.bCanEverTick = true;
	button_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	SetRootComponent(button_mesh);

	interact_point = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractPoint"));
	interact_point->SetupAttachment(GetRootComponent());
	interact_point->bHiddenInGame = true;
	interact_point->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionZone = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionZone"));
	CollisionZone->SetupAttachment(RootComponent);
	CollisionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}


// Called when the game starts
void ASImpleButton::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void ASImpleButton::press_button()
{
	if (!is_pressed) {
		if (pressed_time == 0.f) {
			button_on();
		}
		else {
			button_on();
			GetWorldTimerManager().SetTimer(ButtonTimerHandle, this, &ASImpleButton::button_off, pressed_time, false);
		}
	}
}

void ASImpleButton::button_on()
{
	is_pressed = true;
}

void ASImpleButton::button_off()
{
	is_pressed = false;
}



void ASImpleButton::Interaction(AActor* character)
{	
	if (!is_pressed) {
		auto charact = Cast<AMainCharacter>(character);
		if (charact) {
			charact->PressButtonCharacter(interact_point->GetComponentTransform());
			InteractPressButton();
		}
	}
	press_button();
}

// Called every frame
void ASImpleButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}

