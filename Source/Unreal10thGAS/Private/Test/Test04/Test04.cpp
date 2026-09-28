// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/Test04/Test04.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Test/TestCharacter.h"

void ATest04::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DrawDebugRadius();
}

void ATest04::TestScanAndHit()
{
	UWorld* World = GetWorld();
	if (!World) return;

	DrawDebugRadius(); // 수동 갱신
	const FVector Origin = GetActorLocation();

	// 캡쳐할 타입 지정
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	// 캡쳐 안할 오브젝트 지정
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	if (GetInstigator())
	{
		IgnoreActors.Add(GetInstigator());
	}

	// 오버랩으로 후보들 모집하기
	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::SphereOverlapActors(
		World,
		Origin,
		Radius,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		OverlappingActors
	);

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(ATest04_LoSTrace), false, this);
	TraceParams.AddIgnoredActor(this);
	if (GetInstigator())
	{
		TraceParams.AddIgnoredActor(GetInstigator());
	}

	for (AActor* Overlaped : OverlappingActors)
	{
		if (!IsValid(Overlaped)) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Overlaped);
		if (!TargetASC) continue;

		FVector TargetLocation = Overlaped->GetActorLocation();	// 보정이 필요한 경우가 있을 수 있다.

		FHitResult HitResult;
		const bool bHit = World->LineTraceSingleByChannel(
			HitResult,
			Origin,
			TargetLocation,
			ECC_Camera,
			TraceParams
		);

		const bool bConfiremed = bHit && (HitResult.GetActor() == Overlaped);	// 목표로 한 대상과 라인트레이스에 성공
		const FVector TraceEnd = bHit ? HitResult.ImpactPoint : TargetLocation;
		DrawDebugLine(World, Origin, TraceEnd, bConfiremed ? FColor::Green : FColor::Red, false, 1.0f, 0, 2.0f);

		if (bConfiremed && HitEffectClass)
		{
			FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);
			EffectContextHandle.AddInstigator(this, this);
			EffectContextHandle.AddHitResult(HitResult, true);

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HitEffectClass, EffectLevel, EffectContextHandle);
			if (SpecHandle.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}

	}
}

void ATest04::TestDebuff()
{
	if (!Target) return;
	if (!DebuffEffectClass) return;
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC) return;

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	EffectContextHandle.AddInstigator(this, this);

	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DebuffEffectClass, EffectLevel, EffectContextHandle);
	if (SpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

}

void ATest04::DrawDebugRadius()
{
	if (UWorld* World = GetWorld())
	{
		FlushPersistentDebugLines(World);
		DrawDebugSphere(World, GetActorLocation(), Radius, 32, FColor::Cyan, true, -1.0f, 0, 1.5f);
	}
}
