#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyActor.generated.h"

UCLASS()
class FAUXRANGER_API AMyActor : public APawn {
  GENERATED_BODY()
	
public:	
  AMyActor();

  virtual void Tick(float DeltaTime) override;

protected:
  virtual void BeginPlay() override;

public:
  UPROPERTY(EditAnywhere) bool EnableROS;

  UPROPERTY() class UTopic* CmdVel;

  UPROPERTY() class UTopic* TickerFR;
  UPROPERTY() class UTopic* TickerFL;
  UPROPERTY() class UTopic* TickerRR;
  UPROPERTY() class UTopic* TickerRL;

  UFUNCTION(BlueprintImplementableEvent, Category = "Callback")
  void TeleopEvent(const float & linear, const float & angular);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishTicks(float ticksFR, float ticksFL, float ticksRR, float ticksRL);
};
