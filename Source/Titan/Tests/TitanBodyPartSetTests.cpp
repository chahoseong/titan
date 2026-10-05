#if WITH_DEV_AUTOMATION_TESTS

#include "Combat/TitanBodyPartSet.h"
#include "Misc/AutomationTest.h"
#include "TitanGameplayTags.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTitanBodyPartSetFindTest, "Titan.BodyPartSet.Find",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTitanBodyPartSetFindTest::RunTest(const FString& Parameters)
{
	// Built in code so the test runs without any project asset
	UTitanBodyPartSet* BodyPartSet = NewObject<UTitanBodyPartSet>();

	FTitanBodyPartEntry& Head = BodyPartSet->BodyParts.AddDefaulted_GetRef();
	Head.BodyPart = TitanGameplayTags::BodyPart_Head;
	Head.Bones = { TEXT("head"), TEXT("neck_01") };

	FTitanBodyPartEntry& LeftArm = BodyPartSet->BodyParts.AddDefaulted_GetRef();
	LeftArm.BodyPart = TitanGameplayTags::BodyPart_Arm_Left;
	LeftArm.Bones = { TEXT("lowerarm_l") };

	const FTitanBodyPartEntry* FoundByHead = BodyPartSet->FindBodyPart(TEXT("head"));
	const FTitanBodyPartEntry* FoundByNeck = BodyPartSet->FindBodyPart(TEXT("neck_01"));
	const FTitanBodyPartEntry* FoundByLowerArm = BodyPartSet->FindBodyPart(TEXT("lowerarm_l"));

	TestTrue(TEXT("A listed bone resolves to its body part"),
		FoundByHead && FoundByHead->BodyPart == TitanGameplayTags::BodyPart_Head);
	TestTrue(TEXT("Every bone of an entry resolves to the same body part"),
		FoundByNeck && FoundByNeck->BodyPart == TitanGameplayTags::BodyPart_Head);
	TestTrue(TEXT("Bones of different entries resolve to different body parts"),
		FoundByLowerArm && FoundByLowerArm->BodyPart == TitanGameplayTags::BodyPart_Arm_Left);
	TestTrue(TEXT("A bone no entry lists resolves to no body part"),
		BodyPartSet->FindBodyPart(TEXT("calf_l")) == nullptr);
	TestTrue(TEXT("No bone resolves to no body part"),
		BodyPartSet->FindBodyPart(NAME_None) == nullptr);

	return true;
}

#endif
