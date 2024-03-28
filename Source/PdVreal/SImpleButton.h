// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionInterface.h"
#include "SImpleButton.generated.h"

UCLASS()
class PDVREAL_API ASImpleButton : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ASImpleButton();
	UFUNCTION()
	void press_button();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	bool is_pressed;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void button_on();
	void button_off();

	UFUNCTION(BlueprintImplementableEvent)
	void InteractPressButton();

private:
	//collision box for add to interact
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionZone;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float pressed_time; // how many button will "on", 0 = forever

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* button_mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* interact_point;
	FTimerHandle ButtonTimerHandle;

public:	
	virtual void Interaction(AActor* character) override;
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE bool GetIsPressed() { return is_pressed; };
		
};
