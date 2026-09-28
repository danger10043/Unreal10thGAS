// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/NetTest02/ANetTestCharacter02_Replication.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void AANetTestCharacter02_Replication::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FString NetInfo = FString::Printf(TEXT("Level : %d\nExp : %.1f\nHealth : %.1f"),
		Level, Exp, Health);
	DrawDebugString(GetWorld(), GetActorLocation(), NetInfo, nullptr, FColor::White, 0.0f, true);
}

void AANetTestCharacter02_Replication::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (TestMappingContext)
			{
				Subsystem->AddMappingContext(TestMappingContext, 1);
			}
		}
	}
}

void AANetTestCharacter02_Replication::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	

	if (UEnhancedInputComponent* Enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Enhanced->BindAction(IA_Test1, ETriggerEvent::Started, this, &AANetTestCharacter02_Replication::Test1);
		Enhanced->BindAction(IA_Test2, ETriggerEvent::Started, this, &AANetTestCharacter02_Replication::Test2);
		Enhanced->BindAction(IA_Test3, ETriggerEvent::Started, this, &AANetTestCharacter02_Replication::Test3);
	}
	
}

void AANetTestCharacter02_Replication::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AANetTestCharacter02_Replication, Level, COND_OwnerOnly);	// 오너에게만 리플리케이션 한다.
	//DOREPLIFETIME(AANetTestCharacter02_Replication, Level);	// 모두에게 리플리케이션
	DOREPLIFETIME_CONDITION(AANetTestCharacter02_Replication, Exp, COND_SimulatedOnly);	// 다른 사람들에게만 리플리케이션 한다.
	DOREPLIFETIME(AANetTestCharacter02_Replication, Health);
}

void AANetTestCharacter02_Replication::OnRepNotify_Level()
{
	const FString Str = FString::Printf(TEXT("서버에서 레벨을 %d로 변경했다고 알리고 있습니다."), Level);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, Str);
}

void AANetTestCharacter02_Replication::TestLevelUp()
{
	if (HasAuthority())
	{
		Level++;
	}
}

void AANetTestCharacter02_Replication::Test1()
{
	UE_LOG(LogTemp, Log, TEXT("Test1"));
	if (HasAuthority())
	{
		Level++;
	}
}

void AANetTestCharacter02_Replication::Test2()
{
	UE_LOG(LogTemp, Log, TEXT("Test2"));
	if (HasAuthority())
	{
		Exp += 1.0f;
	}
}

void AANetTestCharacter02_Replication::Test3()
{
	UE_LOG(LogTemp, Log, TEXT("Test3"));

	if (HasAuthority())
	{
		Health += 10.0f;
	}
}
