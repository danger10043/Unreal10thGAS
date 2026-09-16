// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestEnemyCharacter.h"
#include "Widget/OverHeadWidget.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

ATestEnemyCharacter::ATestEnemyCharacter()
{
	// 카메라를 바라보는 빌보드 회전 처리를 위해 틱 활성화
	PrimaryActorTick.bCanEverTick = true;

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComp"));
	OverHeadWidgetComponent->SetupAttachment(RootComponent);

	OverHeadWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	OverHeadWidgetComponent->SetDrawSize(FVector2D(150.0f, 20.0f));
	OverHeadWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	OverHeadWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ATestEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Log, TEXT("BeginPlay"));
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);	// 타이밍 문제로 추가 처리
		InitializeOverHeadWidget();
	}
}

void ATestEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//UE_LOG(LogTemp, Log, TEXT("PossessedBy"));
	
}

void ATestEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFaceCamera)
	{
		UpdateOverheadWidgetRotation();
	}
}

void ATestEnemyCharacter::InitializeOverHeadWidget()
{
	if (!OverHeadWidgetComponent) return;

	//UE_LOG(LogTemp, Log, TEXT("OverHeadWidgetComponent 있음"));

	if (UUserWidget* UserWidget = OverHeadWidgetComponent->GetUserWidgetObject())
	{
		//UE_LOG(LogTemp, Log, TEXT("UserWidget 있음"));
		if (UOverHeadWidget* OverHeadWidget = Cast<UOverHeadWidget>(UserWidget))
		{
			//UE_LOG(LogTemp, Log, TEXT("UOverHeadWidget 캐스트 성공"));
			OverHeadWidget->InitializeWithAbilitySystem(this);
		}
	}
}

void ATestEnemyCharacter::UpdateOverheadWidgetRotation()
{
	if (!OverHeadWidgetComponent) return;

	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		// 카메라의 전방 벡터와 정확히 마주보는 방향(-CameraForward, 사이각 180도)으로 회전
		const FVector CameraForward = CameraManager->GetCameraRotation().Vector();
		FRotator WidgetRotation = (-CameraForward).Rotation();

		if (bLockWidgetPitch)
		{
			WidgetRotation.Pitch = 0.0f;
		}
		if (bLockWidgetRoll)
		{
			WidgetRotation.Roll = 0.0f;
		}

		OverHeadWidgetComponent->SetWorldRotation(WidgetRotation);
	}
}
