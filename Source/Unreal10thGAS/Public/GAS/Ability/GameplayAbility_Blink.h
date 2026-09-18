// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_Blink.generated.h"

/**
 * 전방으로 순간이동하는 어빌리티
 */
UCLASS()
class UNREAL10THGAS_API UGameplayAbility_Blink : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_Blink();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;

protected:
	// 블링크 목적지 계산 및 충돌 검사
	virtual FVector CalcuateBlinkDestination(const ACharacter* InCharacter, float InDistance) const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Blink")
	void OnBlinkExecuted(const FVector& StartLocation, const FVector& DestinationLocation);

protected:
	// 레벨별 이동거리(테이블 적용 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink|Distance")
	FScalableFloat BlinkDistance;
	
	// 충돌 처리시 Hit지점에서 띄울 여유 거리(벽에 끼이는 것 방지)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink|Collision")
	float CollisionOffset = 25.0f;

	// 바닥과의 초기 접점 및 낮은 장애물 무시용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink|Collision")
	float StepOffset = 25.0f;

	// 이동후 캐릭터를 바닥에 착지 시킬지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink|Collision")
	bool bTraceFloor = true;

	// 바닥 감지 최대 깊이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink|Collision")
	float TraceFloorDistance = 500.0f;

	
};
