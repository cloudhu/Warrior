// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WarriorEnumTypes.h"

/**
 * 
 */
class WARRIOR_API FWarriorCountDownAction : public FPendingLatentAction
{
public:
	FWarriorCountDownAction(const float InTotalCountTime, const float InUpdateInterval, float& InOutRemainingTime, EWarriorCountDownActionOutput& InCountDownOutput,
	                        const FLatentActionInfo& LatentInfo)
		: bNeedToCancel(false)
		  , TotalCountDownTime(InTotalCountTime)
		  , UpdateInterval(InUpdateInterval)
		  , OutRemainingTime(InOutRemainingTime)
		  , CountDownOutput(InCountDownOutput)
		  , ExecutionFunction(LatentInfo.ExecutionFunction)
		  , OutputLink(LatentInfo.Linkage)
		  , CallbackTarget(LatentInfo.CallbackTarget)
		  , ElapsedInterval(0.f)
		  , ElapsedTimeSinceStart(0.f)
	{
	}

	void CancelAction();
	virtual void UpdateOperation(FLatentResponse& Response) override;

private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	float UpdateInterval;
	float& OutRemainingTime;
	EWarriorCountDownActionOutput& CountDownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
};
