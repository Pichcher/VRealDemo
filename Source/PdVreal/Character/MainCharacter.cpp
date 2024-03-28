// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "../InteractionInterface.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMainCharacter::AMainCharacter() :
	DefaultCameraTransitionSpeed(1.f),
	CameraMustLocalTransform(FRotator(-15.f, 0.f, 0.f), FVector(-1300.f, 0.f, 300.f), FVector(1.f, 1.f, 1.f)),
	CameraCurrentLocalTransform(CameraMustLocalTransform),
	CameraMustGlobalTransform(FRotator(-10.f, -90.f, 0.f), FVector(0.f, -1640.f, 100.f), FVector(1.f, 1.f, 1.f)),
	CameraCurrentGlobalTransform(CameraMustGlobalTransform),
	IsCameraLocalTransform(true),
	//Camera trasforms
	CameraLocalTransforms{FTransform(FRotator(-5.f, -90.f, 0.f), FVector(0.f, 640.f, 100.f), FVector(1.f, 1.f, 1.f)), 
	FTransform(FRotator(-30.f, -90.f, 0.f), FVector(0.f, 1240.f, 200.f), FVector(1.f, 1.f, 1.f)), 
	FTransform(FRotator(-40.f, -90.f, 0.f), FVector(0.f, 1640.f, 100.f), FVector(1.f, 1.f, 1.f)), 
	FTransform(FRotator(-10.f, -90.f, 0.f), FVector(0.f, 640.f, 100.f), FVector(1.f, 1.f, 1.f)) },
	CameraDistanceDivider(2000.f),
	Speed(0.f),
	CLTidNow(0),
	CLTidWas(0),
	CharacterState(ECharacterState::ECS_Idle),
	ThingInArms(nullptr),
	PullPushRef(nullptr),
	bCanJump(true),
	GotKey(false),
	bCantJumpMantleSystem(false)
{
	// Set this character to call Tic-20k() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CamBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1.f;
	CameraBoom->bUsePawnControlRotation = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollCam"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetupAttachment(RootComponent);
	CubeMesh->SetRelativeScale3D(FVector(1.f));
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CubeMesh->bHiddenInGame = true;


	// lock rotation by controller
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterMesh = GetMesh();
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateCharacterValues(DeltaTime);
	CalculateCameraPosition(DeltaTime);
	CubeMesh->SetWorldRotation(FQuat(0.f));
}

void AMainCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f)) {
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		switch (CharacterState){
		case ECharacterState::ECS_Idle:
			AddMovementInput(Direction, Value);
			break;
		case ECharacterState::ECS_Interact:
			break;
		case ECharacterState::ECS_Climb:
			break;
		case ECharacterState::ECS_PullPush:
			if (IfCharInFrontOfObj) {
				auto Object = Cast<APullPushObject>(PullPushRef);
				FVector ForceLocation = Object->GetLocationHandedPoint();
				ForceLocation.Y = Object->GetActorLocation().Y;
				float NeedValue = ((Object->GetLocationHandedPoint() - this->GetActorLocation() - NeedLocationHandedPoint)).Length() / 45.f;
				Object->GetMesh()->AddForceAtLocation(Direction * 10000000.f * Value / (1 + NeedValue), ForceLocation);
				if (NeedValue<1.f) {
					if ((Object->GetLocationHandedPoint() - this->GetActorLocation()).Length() > (Object->GetLocationHandedPoint() - this->GetActorLocation() - (Direction * Value)).Length()) {
						AddMovementInput(Direction, Value * NeedValue);
					}
					else if ((Object->GetLocationHandedPoint() - this->GetActorLocation()).Length() > (Object->GetLocationHandedPoint() - this->GetActorLocation() - (Direction * -Value)).Length()) {
						AddMovementInput(Direction, -Value * NeedValue);
					}
				}
				else {
					StopInteract();
				}
			}
			break;
		case ECharacterState::ECS_Cable:
			CanMoveWithCable(this->GetActorLocation() + Direction * (Value * 200.f + 60.f));
			if (bCanMoveWithCable){ //200 = character max speed * 0,5 + char radius of skmesh
				AddMovementInput(Direction, Value * 0.5);
			}
			else {
				//play animation where we cant move cable
			}
			break;
		}
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f)) {
		//search and calculate vector for move forward + move forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		switch (CharacterState) {
		case ECharacterState::ECS_Idle:
			AddMovementInput(Direction, Value);
			break;
		case ECharacterState::ECS_Interact:
			break;
		case ECharacterState::ECS_Climb:
			break;
		case ECharacterState::ECS_PullPush:
			if (!IfCharInFrontOfObj) {
				auto Object = Cast<APullPushObject>(PullPushRef);
				FVector ForceLocation = Object->GetLocationHandedPoint();
				ForceLocation.X = Object->GetActorLocation().X;
				float NeedValue = ((Object->GetLocationHandedPoint() - this->GetActorLocation()-NeedLocationHandedPoint)).Length() / 45.f;
				Object->GetMesh()->AddForceAtLocation(Direction * 10000000.f * Value/(1+NeedValue) , ForceLocation);
				if (NeedValue<1.f) {
					if ((Object->GetLocationHandedPoint() - this->GetActorLocation()).Length() > (Object->GetLocationHandedPoint() - this->GetActorLocation() - (Direction * Value )).Length()) {
						AddMovementInput(Direction, Value*NeedValue);
					}
					else if((Object->GetLocationHandedPoint() - this->GetActorLocation()).Length() > (Object->GetLocationHandedPoint() - this->GetActorLocation() - (Direction * -Value)).Length()) {
						AddMovementInput(Direction, -Value*NeedValue);
					}
				}
				else {
					StopInteract();
				}
			}
			break;
		case ECharacterState::ECS_Cable:
			CanMoveWithCable(this->GetActorLocation() + Direction * (Value * 200.f +60.f));
			if (bCanMoveWithCable) {  //200 = character max speed * 0,5 + char radius of skmesh
				AddMovementInput(Direction, Value * 0.5);
			}
			else {
				//play animation where we cant move cable
			}
			break;
		}
	}
}

void AMainCharacter::StartRestartJump()
{	
	GetWorldTimerManager().SetTimer(JumpCooldownTimerHandle, this, &AMainCharacter::RestartJump, 0.2f, false);
}

void AMainCharacter::RestartJump()
{	
	if (!bCanJump) {
		bCanJump = true;
	}
}

void AMainCharacter::CalculateCharacterValues(float DeltaTime)
{
	// calculate speed
	FVector XYspeed{ GetVelocity() };
	XYspeed.Z = 0;
	Speed = XYspeed.Size();
}

void AMainCharacter::CalculateCameraPosition(float DeltaTime)
{
	if (IsCameraLocalTransform) {
		CameraCurrentLocalTransform.SetLocation(FMath::VInterpTo(CameraCurrentLocalTransform.GetLocation(), CameraMustLocalTransform.GetLocation() + CameraMustLocalTransform.GetLocation() * Speed / CameraDistanceDivider, DeltaTime, CameraMustLocalTransform.GetScale3D().X));
		CameraCurrentLocalTransform.SetRotation(FQuat(FMath::RInterpTo(CameraCurrentLocalTransform.Rotator(), CameraMustLocalTransform.Rotator(), DeltaTime, DefaultCameraTransitionSpeed)));
		//CameraCurrentLocalTransform.SetRotation(CameraMustLocalTransform.GetRotation());
		FollowCamera->SetRelativeLocationAndRotation(CameraCurrentLocalTransform.GetLocation(), CameraCurrentLocalTransform.GetRotation(), false, nullptr, ETeleportType::TeleportPhysics);
}
	else {
		CameraCurrentGlobalTransform.SetLocation(FMath::VInterpTo(CameraCurrentGlobalTransform.GetLocation(), CameraMustGlobalTransform.GetLocation(), DeltaTime, CameraMustGlobalTransform.GetScale3D().X));
		CameraCurrentGlobalTransform.SetRotation(FQuat(FMath::RInterpTo(CameraCurrentGlobalTransform.Rotator(), CameraMustGlobalTransform.Rotator(), DeltaTime, DefaultCameraTransitionSpeed)));
		//CameraCurrentLocalTransform.SetRotation(CameraMustLocalTransform.GetRotation());
		FollowCamera->SetWorldLocationAndRotation(CameraCurrentGlobalTransform.GetLocation(), CameraCurrentGlobalTransform.GetRotation(), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AMainCharacter::TryToInteract()
{
	if (CharacterState == ECharacterState::ECS_Idle) {
		StartInteractBP();
	}
	if (CharacterState == ECharacterState::ECS_Idle) {
		GetWorld()->LineTraceSingleByChannel(OutHitResult, this->GetActorLocation() + this->GetActorForwardVector() * 10.f, this->GetActorLocation() + this->GetActorForwardVector() * 100.f, ECollisionChannel::ECC_Visibility);
		DrawDebugLine(GetWorld(), this->GetActorLocation() + this->GetActorForwardVector() * 10.f, this->GetActorLocation() + this->GetActorForwardVector() * 100.f, FColor::Cyan, false, 3.f);
		if (OutHitResult.bBlockingHit) {
			auto ActorToInteract = Cast<IInteractionInterface>(OutHitResult.GetActor());
			if (ActorToInteract) {
				ActorToInteract->Interaction(this);
			}
		}

	}
	
}

void AMainCharacter::StopInteract()
{
	if (CharacterState != ECharacterState::ECS_Idle) {
		StopInteractBP();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		if (CharacterState == ECharacterState::ECS_PullPush) {
			CharacterState = ECharacterState::ECS_Idle;
		}
	}

	
}

void AMainCharacter::PressButtonCharacter(FTransform MoveTo)
{	

	CharacterState = ECharacterState::ECS_Interact;
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = GetCapsuleComponent();
	UKismetSystemLibrary::MoveComponentTo(GetRootComponent(), FVector(MoveTo.GetLocation().X, MoveTo.GetLocation().Y, GetActorLocation().Z+5.f ), MoveTo.GetRotation().Rotator(), true, true, 0.4f, false, EMoveComponentAction::Type::Move, LatentInfo);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && PressButtonMontage) {
		AnimInstance->Montage_Play(PressButtonMontage);
	}
}


void AMainCharacter::PullPush(APullPushObject* PullPush)
{	

		if (FMath::Abs(OutHitResult.ImpactNormal.Y) > FMath::Abs(OutHitResult.ImpactNormal.X)) { // we can use OutHitResult, because we do this, when pullpushobj in it
				IfCharInFrontOfObj = true;
		}
		else {
				IfCharInFrontOfObj = false;
		}
		PullPush->SetRelativeHandedLocation(OutHitResult.Location);
		CharacterState = ECharacterState::ECS_PullPush;
		PullPushRef = PullPush;
		NeedLocationHandedPoint = PullPush->GetLocationHandedPoint() - this->GetActorLocation();
		GetCharacterMovement()->bOrientRotationToMovement = false;

}


void AMainCharacter::CharacterDied()
{	
	CharacterState = ECharacterState::ECS_Dead;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("U Died"));//возможно сделать какое-нибудь уведомление
	CharacterMesh->SetSimulatePhysics(true);//включаем физику у перса
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));//отключаем управление

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent()) {
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComp->SetGenerateOverlapEvents(false);
	}

	LoadSaveDeath();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CameraManager->StartCameraFade(0.f,1.f,3.f,FColor::Black,false,true);//включаем тень на камере
	FTimerHandle UnusedHandle;
	CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMainCharacter::openmap, 5.f, false);//включаем таймер чтобы потом запустить уровень заново
	
}

void AMainCharacter::openmap()
{
	UGameplayStatics::OpenLevel(this, "NewMap1");
}


void AMainCharacter::CharacterStop(bool condition)
{
	(condition) ? GetCharacterMovement()->DisableMovement() : GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}


void AMainCharacter::PlayAnimTakeKey()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimToTakeKey) {
		AnimInstance->Montage_Play(AnimToTakeKey);
	}
}

void AMainCharacter::PlayAnimUseKey(FTransform MoveTo)
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = GetCapsuleComponent();
	UKismetSystemLibrary::MoveComponentTo(GetRootComponent(), FVector(MoveTo.GetLocation().X, MoveTo.GetLocation().Y, GetActorLocation().Z + 5.f), MoveTo.GetRotation().Rotator(), true, true, 0.4f, false, EMoveComponentAction::Type::Move, LatentInfo);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && UseKeyMontage){
		AnimInstance->Montage_Play(UseKeyMontage);
	}
}


void AMainCharacter::CharacterJump()
{	
	MantleBP();
	if (bCanJump && !bCantJumpMantleSystem) {
		if (CharacterState == ECharacterState::ECS_Idle) {
			bCanJump = false;
			Jump();
		}
	}

}



void AMainCharacter::SetCameraLocalTransformId(int id)
{
	if (id == CLTidNow) {
		CLTidNow = CLTidWas;
		CLTidWas = id;
	}
	else {
		CLTidWas = CLTidNow;
		CLTidNow = id;
	}
	CameraMustLocalTransform = CameraLocalTransforms[CLTidNow];
}

void AMainCharacter::ChangeToWorldCameraTranform(FTransform transform)
{
	if (IsCameraLocalTransform) {
		CameraMustGlobalTransform = transform;
		CameraCurrentGlobalTransform = FollowCamera->GetComponentTransform();
		IsCameraLocalTransform = false;
	}
	else {
		CameraCurrentLocalTransform = FollowCamera->GetRelativeTransform();
		IsCameraLocalTransform = true;
	}
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveRightLeft", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForwardBackward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::CharacterJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::TryToInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMainCharacter::StopInteract);
}
