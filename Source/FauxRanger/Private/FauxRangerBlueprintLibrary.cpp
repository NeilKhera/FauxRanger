#include "FauxRangerBlueprintLibrary.h"
#include "mini/ini.h"
#include "Misc/Paths.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

FString UFauxRangerBlueprintLibrary::GetConfigParam(const FString section, const FString key) {
    FString ConfigPath = FPaths::ProjectDir().Append("/Config/FauxRangerConfig.ini");

    mINI::INIFile file(TCHAR_TO_UTF8(*ConfigPath));
    mINI::INIStructure ini;

    std::string section_str = TCHAR_TO_UTF8(*section);
    std::string key_str     = TCHAR_TO_UTF8(*key);

    bool readSuccess = file.read(ini);
    if (readSuccess) {
        bool hasSection = ini.has(section_str);
        if (hasSection) {
            bool hasKey = ini[section_str].has(key_str);
            if (hasKey) {
                std::string value = ini.get(section_str).get(key_str);
                return FString(value.c_str());
            }
        }
    }
    FString EmptyString;
    return EmptyString;
}

FQuat UFauxRangerBlueprintLibrary::FromRotator(const FRotator& From) {
	return From.Quaternion();
}

float UFauxRangerBlueprintLibrary::GetSurface(const UWorld* World, const FVector2D Point, const bool bDrawDebugLines) {
    if (World) {
        FVector StartLocation{ Point.X, Point.Y, 500000 };    // Raytrace starting point.
        FVector EndLocation{ Point.X, Point.Y, -500000 };     // Raytrace end point.

        // Raytrace for overlapping actors.
        FHitResult HitResult;
        World->LineTraceSingleByObjectType(
            OUT HitResult,
            StartLocation,
            EndLocation,
            FCollisionObjectQueryParams(ECollisionChannel::ECC_Visibility),
            FCollisionQueryParams()
        );

        // Draw debug line.
        if (bDrawDebugLines) {
            FColor LineColor;

            if (HitResult.GetActor()) {
                LineColor = FColor::Red;
            }
            else {
                LineColor = FColor::Green;
            }

            DrawDebugLine(World, StartLocation, EndLocation, LineColor, true, 5.f, 0.f, 10.f);
        }

        // Return Z location.
        if (HitResult.GetActor()) {
            return HitResult.ImpactPoint.Z;
        }
    }

    return 0;
}