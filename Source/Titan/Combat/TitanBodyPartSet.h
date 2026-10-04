#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TitanBodyPartSet.generated.h"

USTRUCT()
struct FTitanBodyPartEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Body Part", meta = (Categories = "Titan.BodyPart"))
	FGameplayTag BodyPart;

	/** Bones whose physics bodies belong to this body part */
	UPROPERTY(EditDefaultsOnly, Category = "Body Part")
	TArray<FName> Bones;
};

/**
 * Divides a skeletal mesh into body parts by listing, for each body part,
 * the bones whose physics bodies belong to it.
 */
UCLASS()
class TITAN_API UTitanBodyPartSet : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Body part that owns the bone, or an empty tag if no body part lists it */
	FGameplayTag FindBodyPart(FName BoneName) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	UPROPERTY(EditDefaultsOnly, Category = "Body Part", meta = (TitleProperty = "BodyPart"))
	TArray<FTitanBodyPartEntry> BodyParts;
};
