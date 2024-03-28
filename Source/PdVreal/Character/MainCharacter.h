// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../PullPushObject.h"
#include "MainCharacter.generated.h"


UENUM()
enum class ECharacterState : uint8 {//characters states for specific reaction in actions like move jump and others
	ECS_Idle UMETA(DisplayName="Idle"),
	ECS_Climb UMETA(DisplayName="Climb"),
	ECS_Interact UMETA(DisplayName="Interact"),
	ECS_Cable UMETA(DisplayName="Cable"),
	ECS_PullPush UMETA(DisplayName = "PullPush"),
	ECS_Dead UMETA(DisplaName="Dead")

};
UCLASS()
class PDVREAL_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	//interaction public funs
	void PullPush(APullPushObject* PullPush);
	void StopInteract(); // also can be called from interact objects
	void PressButtonCharacter(FTransform MoveTo);

	UFUNCTION(BlueprintCallable)
	void CharacterDied();

	void PlayAnimTakeKey();

	void PlayAnimUseKey(FTransform MoveTo);

	UFUNCTION(BlueprintCallable)
	void CharacterStop(bool condition);





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveRight(float Value);
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable)
	void StartRestartJump();
	void RestartJump();

	void CalculateCharacterValues(float DeltaTime);

	void CalculateCameraPosition(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent)
	void CanMoveWithCable(FVector EndPoint);


	UFUNCTION(BlueprintImplementableEvent)
	void StartInteractBP();
	UFUNCTION(BlueprintImplementableEvent)
	void StopInteractBP();

	UFUNCTION(BlueprintImplementableEvent)
	void MantleBP();

	void TryToInteract();


	void openmap();

	

	UFUNCTION(BlueprintImplementableEvent)
	void LoadSaveDeath();

	

	void CharacterJump();


private:
	//CameraSystem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowprivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowprivateAccess = "true"))
		class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FTransform>CameraLocalTransforms;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cube", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CubeMesh;
	FTransform CameraCurrentLocalTransform;
	FTransform CameraMustLocalTransform;
	FTransform CameraCurrentGlobalTransform;
	FTransform CameraMustGlobalTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowprivateAccess = "true"))
	bool IsCameraLocalTransform;

	float DefaultCameraTransitionSpeed;
	float CameraDistanceDivider;
	//Camera Local Trasform id in array
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowprivateAccess = "true"))
	int CLTidNow;
	int CLTidWas;

	// Montages 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* PressButtonMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* pickUpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowprivateAccess = "true"))
	class UAnimMontage* AnimToTakeKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowprivateAccess = "true"))
	class UAnimMontage* UseKeyMontage;

	//Character values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	ECharacterState CharacterState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	AActor* ThingInArms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	AActor* ActorForInteract;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	bool bCanMoveWithCable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	bool bCanJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	bool bCantJumpMantleSystem;
	FTimerHandle JumpCooldownTimerHandle;

	//pull push system
	FHitResult OutHitResult;
	bool IfCharInFrontOfObj;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowprivateAccess = "true"))
	AActor* PullPushRef;
	FVector NeedLocationHandedPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterValues, meta = (AllowprivateAccess = "true"))
	USkeletalMeshComponent* CharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowprivateAccess = "true"))
	bool GotKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowprivateAccess = "true"))
	UStaticMeshComponent* Key_mesh;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetCameraLocalTransformId(int id);

	void ChangeToWorldCameraTranform(FTransform transform);
	FORCEINLINE void SetActorToInteract(AActor* actor) { ActorForInteract = actor; };
	FORCEINLINE bool GetIfCharInFrontOfObj() { return IfCharInFrontOfObj; };

	FORCEINLINE void SetCharacterState(ECharacterState state) { CharacterState = state; };

	FORCEINLINE void SetGotKey(bool condition) { GotKey = condition; };

	FORCEINLINE bool GetGotKey() { return GotKey; };

	FORCEINLINE void SetKey(UStaticMeshComponent* key) { Key_mesh = key; };

	FORCEINLINE UStaticMeshComponent* GetKey() { return Key_mesh; };
};
