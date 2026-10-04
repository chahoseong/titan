#pragma once

#include "NativeGameplayTags.h"

namespace TitanGameplayTags
{
	// Body parts. Arm and Leg are parent tags, so either side can be matched with one tag
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyPart_Head);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyPart_Torso);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyPart_Arm_Left);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyPart_Arm_Right);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyPart_Leg_Left);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(BodyPart_Leg_Right);
}
