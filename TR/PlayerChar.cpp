// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerChar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/InputComponent.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include<thread>
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Classes/Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Camera/TRSpringArmComponent.h"
#include "TRArrows.h"
#include "TR_IncendiaryArrows.h"
#include "TR_ExplosiveArrows.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TR_Interactable_Base.h"

// Sets default values
APlayerChar::APlayerChar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<UTRSpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	/*CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "pelvis");*/
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 300.0f;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(CameraBoom);
	PlayerCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "arrow_anchor");

	Arrow_Direction = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Dir"));
	Arrow_Direction->SetupAttachment(HitBox);

	//Timeline Component.
	static ConstructorHelpers::FObjectFinder<UCurveFloat> AimCurve(TEXT("CurveFloat'/Game/Curves/Aim_Curve.Aim_Curve'"));
	if (AimCurve.Object)
	{
		AimCamCurve = AimCurve.Object;
	}

	AimCamTL = FTimeline{};
	FOnTimelineFloat PlayAimAnimation{};
	PlayAimAnimation.BindUFunction(this, "TickAimTimeline");
	AimCamTL.AddInterpFloat(AimCamCurve, PlayAimAnimation, FName{ TEXT("Aim Camera Transition") });


	//Timeline Component.
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CamCurve1(TEXT("CurveFloat'/Game/Curves/Aim_Curve.Aim_Curve'"));
	if (CamCurve1.Object)
	{
		CamCurve = CamCurve1.Object;
	}

	CamTL = FTimeline{};
	FOnTimelineFloat PlayCamAnimation{};
	PlayCamAnimation.BindUFunction(this, "TickCamTimeline");
	CamTL.AddInterpFloat(CamCurve, PlayCamAnimation, FName{ TEXT("Camera Transition") });


	Interactables = nullptr;
}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();

	//Aim Timeline.
	AimCamTL.SetPropertySetObject(this);
	AimCamTL.SetDirectionPropertyName(FName("AimCamTimelineDir"));
	AimCamTL.SetLooping(false);
	AimCamTL.SetTimelineLength(1.0f);
	AimCamTL.SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	AimCamTL.SetPlaybackPosition(0.0f, true, true);
	AimCamTL.SetPlayRate(1.0f);


	//Camera Timeline.
	CamTL.SetPropertySetObject(this);
	CamTL.SetLooping(false);
	CamTL.SetTimelineLength(1.0f);
	CamTL.SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	CamTL.SetPlaybackPosition(0.0f, true, true);
	CamTL.SetPlayRate(1.0f);


	//Delegate for EventAnyDamage.
	OnTakeAnyDamage.AddDynamic(this, &APlayerChar::ReceiveDamage);


	//Adding enum states to the vector(uint8).
	ArrTypes.emplace_back(uint8(EArrowType::Regular_Arrows));
	ArrTypes.emplace_back(uint8(EArrowType::Incendiary_Arrows));
	ArrTypes.emplace_back(uint8(EArrowType::Explosive_Arrows));


	/*ATRArrows* RegularArrows_BP = Cast<ATRArrows>(Reg_Arrows);
	Reg_Arrows = RegularArrows_BP->BP_Class;*/
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Delta = DeltaTime;

	AimCamTL.TickTimeline(DeltaTime);
	CamTL.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Aim_Start", IE_Pressed, this, &APlayerChar::Aim_Start);
	PlayerInputComponent->BindAction("Aim_Start", IE_Released, this, &APlayerChar::Aim_End);
	PlayerInputComponent->BindAction("Shoot_Arrow", IE_Pressed, this, &APlayerChar::Shoot_Arrow);
	PlayerInputComponent->BindAction("Shoot_Arrow", IE_Released, this, &APlayerChar::Shoot_Released);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerChar::Sprint_Start);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerChar::Sprint_End);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::Interact);
	PlayerInputComponent->BindAction("ToggleArrows", IE_Pressed, this, &APlayerChar::ToggleArrows);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("PlayerYaw", this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("PlayerPitch", this, &ACharacter::AddControllerPitchInput);
}

void APlayerChar::MoveForward(float Value)
{
	bGotForwardInput = (Controller != NULL) && (Value != 0.0f);
	if (bGotForwardInput)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value, false);
		CurrentPlayerState = uint8(EPlayerState::P_Moving);
		IsIdle = true;
		StopIdleAnim();
	}
	if (Value == 0.0f && IsIdle == true && bIsAiming == false)
	{
		SetIdleState_Implementation();
	}
}

void APlayerChar::MoveRight(float Value)
{
	bGotRightInput = (Controller != NULL) && (Value != 0.0f);
	if (bGotRightInput)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value, false);
		CurrentPlayerState = uint8(EPlayerState::P_Moving);
		IsIdle = true;
		StopIdleAnim();
	}
	if (Value == 0.0f && IsIdle == true && bIsAiming == false)
	{
		SetIdleState_Implementation();
	}
}

void APlayerChar::Aim_Start()
{
	if (bCanAim == true)
	{
		bIsAiming = true;
		/*bUseControllerRotationYaw = true;*/
		GetCharacterMovement()->bOrientRotationToMovement = false;
		AimCamTL.Play();
		GetCharacterMovement()->MaxWalkSpeed = 30.0f;
		CurrentPlayerState = uint8(EPlayerState::P_Combat);
		StopIdleAnim();
	}
}

void APlayerChar::Aim_End()
{
	bIsAiming = false;
	/*bUseControllerRotationYaw = false;*/
	GetCharacterMovement()->bOrientRotationToMovement = true;
	AimCamTL.Reverse();
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	IsIdle = true;
}

void APlayerChar::Shoot_Arrow()
{
	if (bIsAiming == true && ArrowCount > 0)
	{
		if (bIsAttacking == true)
		{
			bSaveAttack = true;
		}
		else
		{
			bIsCharging = true;
			bIsAttacking = true;
			Charging();
		}
	}
}

void APlayerChar::Shoot()
{
	SwitchOnArrowType();
	Shoot_Implementation();
	/*ChargedTL.PlayFromStart();*/

}

void APlayerChar::Charging()
{
	if (bIsCharging == true)
	{
		FTimerHandle ChargeHandle;
		if (ArrowSpeed < 400)
		{
			ArrowSpeed += 20;
			GetWorld()->GetTimerManager().SetTimer(ChargeHandle, this, &APlayerChar::Charging, 1.0f, false, 0.05f);
		}
	}
}

void APlayerChar::Shoot_Released()
{
	if (bIsAiming == true && ArrowCount > 0)
	{
		bIsCharging = false;
		bIsAttacking = false;
		PlayAnimMontage(Shoot_MontageP, 1.0f);
		Shoot();
		OnPAttack(this);
	}
}

void APlayerChar::ComboAttack()
{
	if (bSaveAttack == true)
	{
		bSaveAttack = false;
		SwitchCombos();
	}
}

void APlayerChar::ComboReset()
{
	AttackCount = 0;
	bSaveAttack = false;
	bIsAttacking = false;
}

void APlayerChar::SwitchCombos()
{
	switch (AttackCount)
	{
	case 0:
		AttackCount = 1;
		PlayAnimMontage(Shoot_MontageP, 1.0f);
		break;

	case 1:
		AttackCount = 0;
		PlayAnimMontage(Shoot_MontageS, 1.0f);
		break;
	}
}

void APlayerChar::Sprint_Start()
{
	if (bIsAiming == false && bCanSprint == true)
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

void APlayerChar::Sprint_End()
{
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

void APlayerChar::Interact()
{
	if (bCanPickUp == true)
	{
		UWorld* const World = GetWorld();
		if (World == nullptr)
		{
			return;
		}

		FVector StartLocation = GetActorLocation();
		FVector EndLocation = GetActorLocation();
		FQuat Rotation = GetActorRotation().Quaternion();
		TArray<FHitResult> HitResults;
		FCollisionShape MyCollisionSphere = FCollisionShape::MakeSphere(150.0f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		World->SweepMultiByChannel(HitResults, StartLocation, EndLocation, Rotation, ECC_WorldDynamic, MyCollisionSphere);

		for (int i = 0; i < HitResults.Num(); i++)
		{
			class ATR_Interactable_Base* IItems = Cast<ATR_Interactable_Base>(HitResults[i].GetActor());

			if (HitResults[i].GetActor() == IItems && IItems != nullptr)
			{
				PlayAnimMontage(PickUp_Montage);
				Interactables = IItems;
				Interactables->Interact();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickUp_P, Interactables->GetActorLocation());
			}
		}
	}
}

void APlayerChar::ToggleArrows()
{
	for (auto &val : ArrTypes)
	{
		if (CurrentArrowType == 0 && val == uint8(EArrowType::Regular_Arrows))
		{
			SelectedArrow = val;
			CurrentArrowType = 1;
			break;
		}
		if (CurrentArrowType == 1 && val == uint8(EArrowType::Incendiary_Arrows))
		{
			SelectedArrow = val;
			CurrentArrowType = 2;
			break;
		}
		if (CurrentArrowType == 2 && val == uint8(EArrowType::Explosive_Arrows))
		{
			SelectedArrow = val;
			CurrentArrowType = 0;
			break;
		}
	}
}

void APlayerChar::OnPAttack(AActor * OwnerActor)
{
	OnPlayerAttack.Broadcast(this);
}

void APlayerChar::TickAimTimeline(float Value)
{
	float LerpedValueX = FMath::Lerp(0.0f, 300.0f, Value);
	float LerpedValueY = FMath::Lerp(0.0f, 240.0f, Value);
	float LerpedValueZ = FMath::Lerp(0.0f, 280.0f, Value);
	float LerpedVal = FMath::Lerp(90.0f, 45.0f, Value);
	PlayerCamera->FieldOfView = LerpedVal;
	/*CameraBoom->TargetOffset = FVector(LerpedValueX, LerpedValueY, LerpedValueZ);*/
	PlayerCamera->SetRelativeLocation(FVector(LerpedValueX, LerpedValueY, LerpedValueZ));
}

void APlayerChar::PlayAimCameraAnim()
{
}

AHoatCameraModificationVolume * APlayerChar::GetCurrentCameraModificationVolume() const
{
	return CurrentCameraModificationVolume;
}

void APlayerChar::SetCurrentCameraModificationVolume(AHoatCameraModificationVolume * InCurrentCameraModificationVolume)
{
	CurrentCameraModificationVolume = InCurrentCameraModificationVolume;
}

bool APlayerChar::GotMovementInput() const
{
	return bGotForwardInput || bGotRightInput;
}

void APlayerChar::ModifySpringArm()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, 1.0f, false, 1.0f);
	CamTL.SetPlayRate(0.37f);
	CamTL.Play();
	bCanSprint = false;
	Sprint_End();
	GetCharacterMovement()->MaxWalkSpeed = 80.0f;
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->PlayerCameraManager->ViewYawMax = 70.0f;
	PlayerController->PlayerCameraManager->ViewYawMin = -70.0f;
	//float Offset = FMath::FInterpTo(0.0f, 100.0f, Delta * 3.0f, 10.0f);
	//CameraBoom->TargetOffset = FVector(0.0f, 0.0f, Offset);
}

void APlayerChar::TickCamimeline(float Value)
{
	float Offset = FMath::Lerp(0.0f, 60.0f, Value);
	float LagSpeed = FMath::Lerp(3.0f, 40.0f, Value);
	float LagDistance = FMath::Lerp(615.42f, 0.0f, Value);
	CameraBoom->CameraLagSpeed = LagSpeed;
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, Offset);
	CameraBoom->CameraLagMaxDistance = LagDistance;
}

void APlayerChar::SwitchOnArrowType()
{
	if (SelectedArrow == uint8(EArrowType::Regular_Arrows))
	{
		SpawnArrow();
		ArrowCount -= 1;
	}
	else if (SelectedArrow == uint8(EArrowType::Incendiary_Arrows))
	{
		SpawnFireArrow();
		ArrowCount -= 1;
	}
	else if (SelectedArrow == uint8(EArrowType::Explosive_Arrows))
	{
		SpawnExpArrow();
		ArrowCount -= 1;
	}

}

void APlayerChar::SpawnFireArrow()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FVector SpawnLocation = HitBox->GetSocketLocation("arrow_anchor");
	FRotator SpawnRotation = HitBox->GetSocketRotation("arrow_anchor");
	FActorSpawnParameters SpawnParams;
	// Velocity of the arrow when the button is released immediately.
	FVector ProjVelocity_R = PlayerCamera->GetForwardVector() * 2000.0f * (ArrowSpeed);
	// Velocity of the arrow when the buttn is held down for some time and then released.
	FVector ProjVelocity_C = PlayerCamera->GetForwardVector() * 250000.0f;
	SpawnParams.Instigator = this;

	IncendiaryArrow = World->SpawnActor<ATR_IncendiaryArrows>(Incen_Arrows, SpawnLocation, SpawnRotation, SpawnParams);
	ATR_IncendiaryArrows* IncendiaryArrows = Cast<ATR_IncendiaryArrows>(IncendiaryArrow);
	IncendiaryArrows->ProjectileMovement->bRotationFollowsVelocity = true;

	if (bIsCharging == false)
		IncendiaryArrows->ProjectileMovement->Velocity = ProjVelocity_R;
	else
		IncendiaryArrows->ProjectileMovement->Velocity = ProjVelocity_R;
	ArrowSpeed = 0.0f;
}

void APlayerChar::SpawnArrow()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FVector SpawnLocation = HitBox->GetSocketLocation("arrow_anchor");
	FRotator SpawnRotation = HitBox->GetSocketRotation("arrow_anchor");
	FActorSpawnParameters SpawnParams;
	// Velocity of the arrow when the button is released immediately.
	FVector ProjVelocity_R = PlayerCamera->GetForwardVector() * 2000.0f * (ArrowSpeed);
	// Velocity of the arrow when the buttn is held down for some time and then released.
	FVector ProjVelocity_C = PlayerCamera->GetForwardVector() * 250000.0f;
	SpawnParams.Instigator = this;

	RegularArrow = World->SpawnActor<ATRArrows>(Reg_Arrows, SpawnLocation, SpawnRotation, SpawnParams);
	ATRArrows* RegularArrows = Cast<ATRArrows>(RegularArrow);
	RegularArrows->ProjectileMovement->bRotationFollowsVelocity = true;

	if (bIsCharging == false)
		RegularArrows->ProjectileMovement->Velocity = ProjVelocity_R;
	else
		RegularArrows->ProjectileMovement->Velocity = ProjVelocity_R;
	ArrowSpeed = 0.0f;
}

void APlayerChar::SpawnExpArrow()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FVector SpawnLocation = HitBox->GetSocketLocation("arrow_anchor");
	FRotator SpawnRotation = HitBox->GetSocketRotation("arrow_anchor");
	FActorSpawnParameters SpawnParams;
	// Velocity of the arrow when the button is released immediately.
	FVector ProjVelocity_R = PlayerCamera->GetForwardVector() * 2000.0f * (ArrowSpeed);
	// Velocity of the arrow when the buttn is held down for some time and then released.
	FVector ProjVelocity_C = PlayerCamera->GetForwardVector() * 250000.0f;
	SpawnParams.Instigator = this;

	ExplosiveArrow = World->SpawnActor<ATR_ExplosiveArrows>(Exp_Arrows, SpawnLocation, SpawnRotation, SpawnParams);
	ATR_ExplosiveArrows* ExplosiveArrows = Cast<ATR_ExplosiveArrows>(ExplosiveArrow);
	ExplosiveArrows->ProjectileMovement->bRotationFollowsVelocity = true;

	if (bIsCharging == false)
		ExplosiveArrows->ProjectileMovement->Velocity = ProjVelocity_R;
	else
		ExplosiveArrows->ProjectileMovement->Velocity = ProjVelocity_R;
	ArrowSpeed = 0.0f;
}

void APlayerChar::SetIdleState()
{
	CurrentPlayerState = uint8(EPlayerState::P_Idle);
	FTimerHandle DelayHandle_Idle;
}

void APlayerChar::PlayIdleAnim()
{
	if (CurrentPlayerState == uint8(EPlayerState::P_Idle) && bIsAiming == false)
	{
		switch (FMath::RandRange(0, 2))
		{
		case 0:
			PlayAnimMontage(Idle_Montage1);
			break;
		case 1:
			PlayAnimMontage(Idle_Montage2);
			break;
		case 2:
			PlayAnimMontage(Idle_Montage3);
			break;
		}
	}
}

void APlayerChar::StopIdleAnim()
{
	StopAnimMontage(Idle_Montage1);
	StopAnimMontage(Idle_Montage2);
	StopAnimMontage(Idle_Montage3);
}

void APlayerChar::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
}



void APlayerChar::ToughSkin1()
{
	Defense = 3.0f;
	Skills.bIsSkillEnabled = false;
}

void APlayerChar::ToughSkin2()
{
	Defense = 5.0f;
	Skills.bIsSkillEnabled = false;
}

void APlayerChar::ToughSkin3()
{
	Defense = 7.0f;
	Skills.bIsSkillEnabled = false;
}

void APlayerChar::UnlockChargedShot()
{
	bIsCharging = true;
}

