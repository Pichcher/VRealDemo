// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "PullPushObject.generated.h"

UCLASS()
class PDVREAL_API APullPushObject : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APullPushObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	//skeletal mesh for item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* LocalHandedPoint;

public:	
	virtual void Interaction(AActor* character) override;
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE void SetRelativeHandedLocation(FVector loc) {
		LocalHandedPoint->SetWorldLocation(loc, false, nullptr, ETeleportType::None);
		LocalHandedPoint->SetupAttachment(ObjectMesh);
	};
	FORCEINLINE FVector GetLocationHandedPoint() { return LocalHandedPoint->GetComponentLocation(); };
	FORCEINLINE UStaticMeshComponent* GetMesh() { return ObjectMesh; };
};
