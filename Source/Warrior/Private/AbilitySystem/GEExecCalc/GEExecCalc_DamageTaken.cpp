// CloudHu:604746493@qq.com All Rights Reserved


#include "AbilitySystem/GEExecCalc/GEExecCalc_DamageTaken.h"

#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAttributeSet.h"

struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FWarriorDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false)
	}
};

static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                     FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);

	float BaseDamage = 0.f;
	int32 LightAttackCombo = 0;
	int32 HeavyAttackCombo = 0;
	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
		}

		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			LightAttackCombo = TagMagnitude.Value;
		}

		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			HeavyAttackCombo = TagMagnitude.Value;
		}
	}
	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters, TargetDefensePower);

	//轻击-连击加成
	if (LightAttackCombo != 0)
	{
		const float DamageIncreasePercentLight = (LightAttackCombo - 1) * 0.05 + 1.0f;
		BaseDamage *= DamageIncreasePercentLight;
	}

	//重击-连击加成
	if (HeavyAttackCombo != 0)
	{
		const float DamageIncreasePercentHeavy = HeavyAttackCombo * 0.15f + 1.f;
		BaseDamage *= DamageIncreasePercentHeavy;
	}

	if (const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower; FinalDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetWarriorDamageCapture().DamageTakenProperty, EGameplayModOp::Override, FinalDamageDone));
	}
}
