// CloudHu:604746493@qq.com All Rights Reserved


#include "WarriorTypes/WarriorStructTypes.h"

bool FWarriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
