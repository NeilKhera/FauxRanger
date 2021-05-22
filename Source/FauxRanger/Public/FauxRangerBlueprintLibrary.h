#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FauxRangerBlueprintLibrary.generated.h"

UCLASS()
class FAUXRANGER_API UFauxRangerBlueprintLibrary : public UBlueprintFunctionLibrary {

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Configuration")
	static FString GetConfigParam(const FString section, const FString key);

	UFUNCTION(BlueprintPure, Category = "Conversions")
	static FQuat FromRotator(const FRotator& From);

	UFUNCTION(BlueprintPure, Category = "Geometry")
	static float GetSurface(const UWorld* World, const FVector2D Point, const bool bDrawDebugLines = false);
};
