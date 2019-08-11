// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Camera/TRCameraInterface.h"
#include "Camera/TRPlayerCharacterInterface.h"
#include<vector>
#include "PlayerChar.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UTimeline;
class UTimelineComponent;
class UAnimMontage;
class UArrowComponent;
class UBoxComponent;
class ATRArrows;
class ATR_IncendiaryArrows;
class ATR_ExplosiveArrows;
class UTexture2D;
class ATR_Interactable_Base;

USTRUCT(BlueprintType)
struct FSkillsData
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	* Constructor.
	*/

	FSkillsData()
	{
		Skill_Name = FText::FromString("Skill Name");
		Skill_Description = FText::FromString("Please Enter a description for the skill.");
		Skill_Category = FText::FromString("Enter Category");
	}


	// Name of the skill.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		FText Skill_Name;

	// Thumbnail of the respective skill.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		UTexture2D* Skill_Thubnail;

	// Description of the respective skill.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		FText Skill_Description;

	// ID of the respective skill.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		int32 Skill_ID;

	// To which the category the skill belongs to.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		FText Skill_Category;

	// Boolean used to check if the skill can be used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		bool bIsSkillEnabled = true;


	// Operator overloading to compare data to notify unreal c++ of the item being used or deleted.
	bool operator==(const FSkillsData& Skill) const
	{
		if (Skill_ID == Skill.Skill_ID)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};


UENUM()
enum class EArrowType : uint8
{
	Regular_Arrows,
	Incendiary_Arrows,
	Explosive_Arrows
};


UENUM()
enum class EPlayerState : uint8
{
	P_Idle,
	P_Moving,
	P_Combat,
	P_UnderAttck,
	P_HealthLow,
	P_Recovering
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackSignature, AActor*, OwnerActor);

UCLASS()
class TR_API APlayerChar : public ACharacter, public ITRCameraInterface, public ITRPlayerCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerChar();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* PlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReaDWrite)
		UArrowComponent* Arrow_Direction;

	// Player Health.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health = 100.0f * Defense;

	// Player Defense - Used to toughen character upon earning skill points.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Defense = 1.0f;

	// Skill Points.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SkillPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSaveAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 AttackCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ArrowCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ATRArrows> Arrows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanAim = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanSprint = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsIdle = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsCharging = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanPickUp = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanInteract = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAutoFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ATR_Interactable_Base* Interactables;

	float Delta;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ArrowSpeed = 0.0f;


	/*
	* vector<uint8> to store the enums.
	*/

	std::vector<uint8> ArrTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentArrowType;

	uint8 SelectedArrow;

	/*
	* Current Player State.
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 CurrentPlayerState;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ATRArrows> Reg_Arrows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ATR_IncendiaryArrows> Incen_Arrows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ATR_ExplosiveArrows> Exp_Arrows;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AActor* RegularArrow;
	AActor* IncendiaryArrow;
	AActor* ExplosiveArrow;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* PickUp_P;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	*	Player Actions.
	*/
	UFUNCTION(BlueprintCallable)
		void MoveForward(float Value);

	UFUNCTION(BlueprintCallable)
		void MoveRight(float Value);

	UFUNCTION(BlueprintCallable)
		void Aim_Start();

	UFUNCTION(BlueprintCallable)
		void Aim_End();

	UFUNCTION(BlueprintCallable)
		void Shoot_Arrow();

	UFUNCTION()
		void Shoot();

	UFUNCTION()
		void Charging();

	UFUNCTION()
		void Shoot_Released();

	UFUNCTION(BlueprintCallable)
		void ComboAttack();

	UFUNCTION(BlueprintCallable)
		void ComboReset();

	UFUNCTION()
		void SwitchCombos();

	UFUNCTION(BlueprintCallable)
		void Sprint_Start();

	UFUNCTION(BlueprintCallable)
		void Sprint_End();

	UFUNCTION(BlueprintCallable)
		void Interact();

	UFUNCTION(BlueprintImplementableEvent)
		void Shoot_Implementation();

	UFUNCTION(BlueprintCallable)
		void ToggleArrows();


	/*
	*	Animation Montages.
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AimStart_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AimEnd_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Shoot_MontageP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Shoot_MontageS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Idle_Montage1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Idle_Montage2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Idle_Montage3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* PickUp_Montage;


	/*
	*	OnPlayerAttck delegate.
	*/

	UPROPERTY(BlueprintAssignable)
		FOnAttackSignature OnPlayerAttack;

	UFUNCTION(BlueprintCallable)
		void OnPAttack(AActor* OwnerActor);


	/*
	*	Aim Camera Timeline.
	*/

	//Timneline Component to Play the Aim camera transition animation.
	UPROPERTY()
		FTimeline AimCamTL;

	//Function which gets called from the Timer to call anim Timeline.
	UFUNCTION()
		void TickAimTimeline(float Value);

	UPROPERTY()
		TEnumAsByte<ETimelineDirection::Type> AimCamTimelineDir;

	//Curve float for the timeline component.
	UPROPERTY()
		UCurveFloat* AimCamCurve;

	UPROPERTY()
		FTimerHandle TlineFinishHandle_Aim;

	//Called when the timeline finishes playing.
	UFUNCTION()
		void PlayAimCameraAnim();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }

	virtual AHoatCameraModificationVolume* GetCurrentCameraModificationVolume() const override;
	virtual void SetCurrentCameraModificationVolume(AHoatCameraModificationVolume* InCurrentCameraModificationVolume) override;
	virtual bool GotMovementInput() const override;

	UFUNCTION()
		void ModifySpringArm();


private:

	AHoatCameraModificationVolume* CurrentCameraModificationVolume;

	bool bGotForwardInput;
	bool bGotRightInput;

	UPROPERTY()
		FSkillsData Skills;


public:

	/*
	* SpringArm Timeline.
	*/

	//Timeline Component to Play the modify spring arm transition animation.
	UPROPERTY()
		FTimeline CamTL;

	//Function which gets called from the Timer to call anim Timeline.
	UFUNCTION()
		void TickCamimeline(float Value);

	//Curve float for the timeline component.
	UPROPERTY()
		UCurveFloat* CamCurve;

	UPROPERTY()
		FTimerHandle TlineFinishHandle_Cam;


	/*
	* Spawn Arrows.
	*/

	UFUNCTION()
		void SwitchOnArrowType();

	UFUNCTION()
		void SpawnFireArrow();

	UFUNCTION()
		void SpawnArrow();

	UFUNCTION()
		void SpawnExpArrow();



	/*
	* Play animations when the character is idle for a while.
	*/

	UFUNCTION(BlueprintCallable)
		void SetIdleState();

	UFUNCTION(BlueprintImplementableEvent)
		void SetIdleState_Implementation();

	UFUNCTION(BlueprintCallable)
		void PlayIdleAnim();

	UFUNCTION()
		void StopIdleAnim();


protected:
	/*
	* Receive Damage Delegate.
	*/

	UFUNCTION()
		void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser);


	/*
	* TouchSkin skill - Defense Boost.
	*/

	UFUNCTION(BlueprintCallable)
		void ToughSkin1();

	UFUNCTION(BlueprintCallable)
		void ToughSkin2();

	UFUNCTION(BlueprintCallable)
		void ToughSkin3();


	/*
	* Unlock Charged Shot - Skill.
	*/

	UFUNCTION(BlueprintCallable)
		void UnlockChargedShot();
};
