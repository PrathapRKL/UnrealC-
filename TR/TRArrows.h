// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TRArrows.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class TR_API ATRArrows : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATRArrows();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* ArrowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* CollSP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystem* Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage = 20.0f;

	TSubclassOf<class ATRArrows> BP_Class;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void OnOverlapBegin(UPrimitiveComponent* OwnerComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool Teleport, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable)
		void OnOverlapEnd(UPrimitiveComponent* OwnerComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
