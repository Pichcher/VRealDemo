// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionZoneGlobalCS.generated.h"

UCLASS()
class PDVREAL_API ACollisionZoneGlobalCS : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollisionZoneGlobalCS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	//collision box for transform zone
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FTransform MustTransformIn;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
