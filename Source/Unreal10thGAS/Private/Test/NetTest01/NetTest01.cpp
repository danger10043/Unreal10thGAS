// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/NetTest01/NetTest01.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Test/NetTest01/NetTestCharacter01_Connection.h"

// Sets default values
ANetTest01::ANetTest01()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	OverlapCollision = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapCollision"));
	OverlapCollision->SetupAttachment(RootComponent);
	OverlapCollision->SetSphereRadius(400.0f);

}

// Called when the game starts or when spawned
void ANetTest01::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetTest01::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(GetWorld(), GetActorLocation(), OverlapCollision->GetScaledSphereRadius(), 32, FColor::Yellow);

	if (HasAuthority())
	{
		// 서버에서만 처리하겠다.
		AActor* NextOwner = nullptr;
		float MinDistanceSquared = OverlapCollision->GetScaledSphereRadius() * OverlapCollision->GetScaledSphereRadius();
		TArray<AActor*> OverlapActors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetTestCharacter01_Connection::StaticClass(), OverlapActors);
		for (AActor* Actor : OverlapActors)
		{
			float DistanceSquared = GetSquaredDistanceTo(Actor);
			if (MinDistanceSquared > DistanceSquared)
			{
				MinDistanceSquared = DistanceSquared;
				NextOwner = Actor;
			}
		}
		if (GetOwner() != NextOwner)
		{
			SetOwner(NextOwner);
			FString OwnerName = GetOwner() ? GetOwner()->GetName() : TEXT("오너 없음");
			UE_LOG(LogTemp, Log, TEXT("새 오너 : %s"), *OwnerName);
		}
		
	}

	const FString LocalRoleString = UEnum::GetValueAsString(GetLocalRole());
	const FString RemoteRoleString = UEnum::GetValueAsString(GetRemoteRole());

	const FString OwnerString = GetOwner() ? GetOwner()->GetName() : TEXT("오너 없음");
	const FString ConnectionString = GetNetConnection() ? TEXT("커넥션 있음") : TEXT("커넥션 없음");

	const FString NetInfo = FString::Printf(TEXT("Owner : %s\nConnection : %s\nLocalRole : %s\nRemoteRole : %s"),
		*OwnerString, *ConnectionString, *LocalRoleString, *RemoteRoleString);
	DrawDebugString(GetWorld(), GetActorLocation(), NetInfo, nullptr, FColor::White, 0.0f, true);
}

void ANetTest01::ApplyTargetToOwner()
{
	if (!Target) return;

	if (HasAuthority())
	{
		SetOwner(Target);
		UE_LOG(LogTemp, Log, TEXT("%s가 오너로 설정되었습니다."), *Target->GetName());
	}
}

