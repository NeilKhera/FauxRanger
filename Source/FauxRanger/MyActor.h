#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Templates/SubclassOf.h"

#include "MyActor.generated.h"

UCLASS()
class FAUXRANGER_API AMyActor : public APawn {
  
	GENERATED_BODY()

public:
	UPROPERTY() class UTopic* topic_sun_seeker;
	UPROPERTY() class UTopic* topic_goal;
	UPROPERTY() class UTopic* topic_cmd_vel;

	UPROPERTY() class UTopic* topic_wheels;
	UPROPERTY() class UTopic* topic_odom;
	UPROPERTY() class UTopic* topic_IMU;
	UPROPERTY() class UTopic* topic_clock;

private:
	uint32 imu_seq = 0;
	uint32 odom_seq = 0;

	bool paused = false;
	bool update_sun = false;
	bool spawn_waypoint = false;

	FVector sun_vector;
	FVector waypoint_location;
	FRotator waypoint_rotation;
	FVector waypoint_scale;
  
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<AActor> WaypointActor;

public:
	AMyActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Callback")
	void VectorEvent(const FVector & angles);

	UFUNCTION(BlueprintImplementableEvent, Category = "Callback")
	void SpawnEvent(const FVector & location, const FRotator & rotation, const FVector & scale);

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

	UFUNCTION(BlueprintCallable, Category = "Publisher")
	void PublishClock();

private:
	FString GetTopic(const FString key, const FString default_topic);

};