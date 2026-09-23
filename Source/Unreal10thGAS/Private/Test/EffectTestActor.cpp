
#include "Test/EffectTestActor.h"
#include "AbilitySystemComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "GAS/StatAttributeSet.h"
#include "NativeGameplayTags.h"
#include "Player/PlayerCharacter.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_EffectTest_Magnitude, "Data.EffectTest.Magnitude")

AEffectTestActor::AEffectTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AEffectTestActor::ApplyGameplayEffect()
{
	if (!GameplayEffectClass || !FMath::IsFinite(EffectLevel) || EffectLevel <= 0.0f || !FMath::IsFinite(Magnitude))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EffectTestActor::ApplyGameplayEffect - Effect Class, Level, Magnitude 를 확인하세요.")
		);
		return;
	}

	UAbilitySystemComponent* ASC = PrepareTarget();
	if (!ASC) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);
	Context.AddInstigator(this, this);

	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GameplayEffectClass, EffectLevel, Context);
	if (!Spec.IsValid()) return;

	Spec.Data->SetSetByCallerMagnitude(TAG_EffectTest_Magnitude, Magnitude);
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

void AEffectTestActor::RemoveAllEffects()
{
	UAbilitySystemComponent* ASC = PrepareTarget();
	if (!ASC) return;

	ASC->RemoveActiveEffects(FGameplayEffectQuery());
}

void AEffectTestActor::ResetTarget()
{
	UAbilitySystemComponent* ASC = PrepareTarget();
	if (!ASC) return;

	const TWeakObjectPtr<APlayerCharacter> TargetKey(Target.Get());
	const TMap<FGameplayAttribute, float> SavedValues = InitialValues.FindChecked(TargetKey);

	ASC->RemoveActiveEffects(FGameplayEffectQuery());

	const TArray<FGameplayAttribute> MaxAttributes =
	{
		UStatAttributeSet::GetMaxHealthAttribute(),
		UStatAttributeSet::GetMaxStaminaAttribute(),
		UStatAttributeSet::GetMaxJumpChargeAttribute()
	};

	for (const FGameplayAttribute& Attribute : MaxAttributes)
	{
		if (const float* Value = SavedValues.Find(Attribute))
		{
			if (ASC->HasAttributeSetForAttribute(Attribute))
			{
				ASC->SetNumericAttributeBase(Attribute, *Value);
			}
		}
	}

	for (const TPair<FGameplayAttribute, float>& Pair : SavedValues)
	{
		if (!MaxAttributes.Contains(Pair.Key) && ASC->HasAttributeSetForAttribute(Pair.Key))
		{
			ASC->SetNumericAttributeBase(Pair.Key, Pair.Value);
		}
	}
}

UAbilitySystemComponent* AEffectTestActor::PrepareTarget()
{
	if (!GetWorld() || !GetWorld()->IsGameWorld())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EffectTestActor::PrepareTarget - 플레이 중에 실행하세요.")
		);
		return nullptr;
	}

	if (!IsValid(Target) || Target->GetWorld() != GetWorld() || !Target->HasAuthority())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EffectTestActor::PrepareTarget - Target이 유효하지 않습니다.")
		);
		return nullptr;
	}

	UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent();
	if (!IsValid(ASC) || !IsValid(Target->GetStatAttributeSet()))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EffectTestActor::PrepareTarget - Target의 ASC가 유효하지 않습니다.")
		);
		return nullptr;
	}
	
	const TWeakObjectPtr<APlayerCharacter> TargetKey(Target.Get());
	if (!InitialValues.Contains(TargetKey))
	{
		TArray<FGameplayAttribute> Attributes;
		ASC->GetAllAttributes(Attributes);

		TMap<FGameplayAttribute, float>& Values = InitialValues.Add(TargetKey);
		for (const FGameplayAttribute& Attribute : Attributes)
		{
			Values.Add(Attribute, ASC->GetNumericAttributeBase(Attribute));
		}
	}
	return ASC;
}

