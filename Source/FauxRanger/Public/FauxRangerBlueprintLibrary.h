#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FauxRangerBlueprintLibrary.generated.h"

UCLASS()
class FAUXRANGER_API UFauxRangerBlueprintLibrary : public UBlueprintFunctionLibrary {

	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Conversions")
	static FQuat FromRotator(const FRotator& From);
};
