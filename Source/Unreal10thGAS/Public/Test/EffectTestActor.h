#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttributeSet.h"
#include "EffectTestActor.generated.h"

class APlayerCharacter;
class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class UNREAL10THGAS_API AEffectTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEffectTestActor();

	UPROPERTY(EditAnywhere, Category = "Effect Test")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Effect Test", meta = (ClampMin = "0.01"))
	float EffectLevel = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Effect Test")
	float Magnitude = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Effect Test")
	TObjectPtr<APlayerCharacter> Target = nullptr;

	UFUNCTION(CallInEditor, Category = "Effect Test")
	void ApplyGameplayEffect();

	UFUNCTION(CallInEditor, Category = "Effect Test")
	void RemoveAllEffects();

	UFUNCTION(CallInEditor, Category = "Effect Test")
	void ResetTarget();

private:
	UAbilitySystemComponent* PrepareTarget();

	TMap<TWeakObjectPtr<APlayerCharacter>, TMap<FGameplayAttribute, float>> InitialValues;

};
