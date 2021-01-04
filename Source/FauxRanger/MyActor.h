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

  UFUNCTION(BlueprintImplementableEvent, Category = "Callback")
  void TeleopEvent(const float & linear, const float & angular);

  UFUNCTION(BlueprintCallable, Category = "ROS")
  void Pause(const bool pause = true);

  UFUNCTION(BlueprintCallable, Category = "ROS")
  void InitializeTopics();

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishWheelData(int32 rear_left, int32 rear_right, int32 front_left, int32 front_right);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishOdometry(FVector position, FQuat orientation, FVector linear_velocity, FVector angular_velocity);

  UFUNCTION(BlueprintCallable, Category = "Publisher")
  void PublishIMU(FQuat orientation, FVector angular_velocity, FVector linear_acceleration);
};
