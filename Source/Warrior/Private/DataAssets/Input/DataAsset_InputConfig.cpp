// CloudHu:604746493@qq.com All Rights Reserved


#include "DataAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FWarriorInputActionConfig& NativeInputAction : NativeInputActions)
	{
		if (NativeInputAction.InputTag == InInputTag && NativeInputAction.InputAction)
		{
			return NativeInputAction.InputAction;
		}
	}
	return nullptr;
}
