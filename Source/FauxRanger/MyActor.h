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

private:
  uint32 IMUSeq;

public:
  UPROPERTY(EditAnywhere) bool EnableROS;

  UPROPERTY() class UTopic* CmdVel;
  UPROPERTY() class UTopic* WheelVelocityFR;
  UPROPERTY() class UTopic* WheelVelocityFL;
  UPROPERTY() class UTopic* WheelVelocityRR;
  UPROPERTY() class UTopic* WheelVelocityRL;
  UPROPERTY() class UTopic* IMU;

  UFUNCTION(BlueprintImplementableEvent, Category = "Callback")
  void TeleopEvent(const float & linear, const float & angular);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishWheelVelocities(float velocityFR, float velocityFL, float velocityRR, float velocityRL);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishIMU(FQuat orientation, FVector angular_velocity, FVector linear_acceleration);
};
