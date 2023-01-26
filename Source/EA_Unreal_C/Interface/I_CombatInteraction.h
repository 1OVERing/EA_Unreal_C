#pragma once

#include "CoreMinimal.h"
#include "I_CombatInteraction.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_CombatInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EA_UNREAL_C_API II_CombatInteraction
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Interface")
	void PlayKnockBack();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void PlayStiffen();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	bool PlayCatchAttack(UAnimMontage* montage,FName sectionName);
};
