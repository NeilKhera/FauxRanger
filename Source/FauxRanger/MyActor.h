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
  bool paused;

  uint32 imu_seq;
  uint32 odom_seq;

public:
  UPROPERTY() class UTopic* topic_cmd_vel;
  UPROPERTY() class UTopic* topic_wheels;
  UPROPERTY() class UTopic* topic_imu;
  UPROPERTY() class UTopic* topic_odom;

  UFUNCTION(BlueprintCallable, Category = "ROS")
  void Pause(const bool _Pause = true);

  UFUNCTION(BlueprintCallable, Category = "ROS")
  void InitializeTopics();

  UFUNCTION(BlueprintImplementableEvent, Category = "Callback")
  void TeleopEvent(const float & linear, const float & angular);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishWheelData(float velocityFR, float velocityFL, float velocityRR, float velocityRL);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishIMU(FQuat orientation, FVector angular_velocity, FVector linear_acceleration);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishOdometry(FVector position, FQuat orientation);
};
