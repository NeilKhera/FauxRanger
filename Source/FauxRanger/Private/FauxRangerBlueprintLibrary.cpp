#include "FauxRangerBlueprintLibrary.h"

FQuat UFauxRangerBlueprintLibrary::FromRotator(const FRotator& From)
{
	return From.Quaternion();
}