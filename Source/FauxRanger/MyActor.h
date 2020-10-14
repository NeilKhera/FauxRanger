#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyActor.generated.h"

UCLASS()
class FAUXRANGER_API AMyActor : public APawn
{
	GENERATED_BODY()
	
public:	
	AMyActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	class UTopic* ExampleTopic;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Output")
	void TeleopEvent(const float & linear, const float & angular);
};
