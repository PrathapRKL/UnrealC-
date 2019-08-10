// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TR_Interactable_Base.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;

UCLASS()
class TR_API ATR_Interactable_Base : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATR_Interactable_Base();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* InteractableMesh;


	/*
	* WidgetComponent to display a widget/UI when the player enters the trigger (CollisionComponent).
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* InteractWidget;

	UPROPERTY(EditAnywhere)
		FString ItemName;

	/*
	* Text to appear when the player character enters the trigger (CollisionComponent).
	*/

	UPROPERTY(EditAnywhere)
		FString InteractableText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ItemCount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	/*
	* Function to be called if the item is being used.
	*/

	UFUNCTION(BlueprintCallable)
		void Use();

	UFUNCTION(BLueprintCallable)
		void Interact();


	/*
	* OnBeginOverlap and OnEndOverlap delegate functions - CollisionComponent.
	*/

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
