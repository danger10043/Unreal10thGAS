// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/NetTest01/NetTestCharacter01_Connection.h"

void ANetTestCharacter01_Connection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FString LocalRoleString = UEnum::GetValueAsString(GetLocalRole());
	const FString RemoteRoleString = UEnum::GetValueAsString(GetRemoteRole());

	const FString OwnerString = GetOwner() ? GetOwner()->GetName() : TEXT("오너 없음");
	const FString ConnectionString = GetNetConnection() ? TEXT("커넥션 있음") : TEXT("커넥션 없음");
		
	const FString NetInfo = FString::Printf(TEXT("Owner : %s\nConnection : %s\nLocalRole : %s\nRemoteRole : %s"), 
		*OwnerString, *ConnectionString, *LocalRoleString, *RemoteRoleString);
	DrawDebugString(GetWorld(), GetActorLocation(), NetInfo, nullptr, FColor::White, 0.0f, true);
}
