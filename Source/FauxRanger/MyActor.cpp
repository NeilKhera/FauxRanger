#include "MyActor.h"
#include "ROSTime.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"

#include "ROSIntegration/Public/sensor_msgs/Imu.h"
#include "ROSIntegration/Public/nav_msgs/Odometry.h"
#include "ROSIntegration/Public/rasm_msgs/Waypoint.h"

#include "ROSIntegration/Public/std_msgs/Header.h"
#include "ROSIntegration/Public/std_msgs/Int32MultiArray.h"

#include "ROSIntegration/Public/geometry_msgs/Pose.h"
#include "ROSIntegration/Public/geometry_msgs/Twist.h"
#include "ROSIntegration/Public/geometry_msgs/PoseWithCovariance.h"
#include "ROSIntegration/Public/geometry_msgs/TwistWithCovariance.h"

AMyActor::AMyActor() {
  PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay() {
  Super::BeginPlay();

  paused = false;
  odom_seq = 0;
  imu_seq = 0;
}

void AMyActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

void AMyActor::Pause(const bool pause) {
  paused = pause;
}

float AMyActor::GetSurface(FVector2D Point, bool bDrawDebugLines) {
    UWorld* World { this->GetWorld() };

    if (World) {
        FVector StartLocation { Point.X, Point.Y, 10000 };    // Raytrace starting point.
        FVector EndLocation { Point.X, Point.Y, -10000 };     // Raytrace end point.

        // Raytrace for overlapping actors.
        FHitResult HitResult;
        World->LineTraceSingleByObjectType(
            OUT HitResult,
            StartLocation,
            EndLocation,
            FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
            FCollisionQueryParams()
        );

        // Draw debug line.
        if (bDrawDebugLines) {
            FColor LineColor;

            if (HitResult.GetActor()) {
                LineColor = FColor::Red;
            } else {
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

void AMyActor::InitializeTopics() {
    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());

    if (rosinst && rosinst->bConnectToROS) {
        // Initialize a topic
        topic_goal = NewObject<UTopic>(UTopic::StaticClass());
        topic_cmd_vel = NewObject<UTopic>(UTopic::StaticClass());
        topic_wheels = NewObject<UTopic>(UTopic::StaticClass());
        topic_odom = NewObject<UTopic>(UTopic::StaticClass());

        topic_goal->Init(rosinst->ROSIntegrationCore, TEXT("/rasm/goal_command"), TEXT("rasm_msgs/Waypoint"));
        topic_cmd_vel->Init(rosinst->ROSIntegrationCore, TEXT("/moon_ranger_velocity_controller/cmd_vel"), TEXT("geometry_msgs/Twist"));
        topic_wheels->Init(rosinst->ROSIntegrationCore, TEXT("/wheels"), TEXT("std_msgs/Int32MultiArray"));
        topic_odom->Init(rosinst->ROSIntegrationCore, TEXT("/moon_ranger_velocity_controller/odom"), TEXT("nav_msgs/Odometry"));

        topic_wheels->Advertise();
        topic_odom->Advertise();

        // Create a std::function callback object
        std::function<void(TSharedPtr<FROSBaseMsg>)> GoalCallback = [this](TSharedPtr<FROSBaseMsg> msg) -> void {
            auto Concrete = StaticCastSharedPtr<ROSMessages::rasm_msgs::Waypoint>(msg);
            if (Concrete.IsValid()) {
                // Spawn Waypoint
            }
            return;
        };

        std::function<void(TSharedPtr<FROSBaseMsg>)> CmdVelCallback = [this](TSharedPtr<FROSBaseMsg> msg) -> void {
            auto Concrete = StaticCastSharedPtr<ROSMessages::geometry_msgs::Twist>(msg);
            if (Concrete.IsValid()) {
                this->TeleopEvent(Concrete->linear.x, Concrete->angular.z);
            }
            return;
        };

        // Subscribe to the topic
        topic_goal->Subscribe(GoalCallback);
        topic_cmd_vel->Subscribe(CmdVelCallback);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Setting up ROS instance failed!"));
    }
}

void AMyActor::PublishWheelData(int32 rear_left, int32 rear_right, int32 front_left, int32 front_right) {
    if (paused) {
        return;
    }

    if (topic_wheels->IsAdvertising()) {
        ROSMessages::std_msgs::MultiArrayDimension MessageDim;

        MessageDim.label = FString(TEXT("wheels"));
        MessageDim.size = 4;
        MessageDim.stride = 4;

        TSharedPtr<ROSMessages::std_msgs::Int32MultiArray> MessageInt32MultiArray(new ROSMessages::std_msgs::Int32MultiArray());

        MessageInt32MultiArray->layout.dim.Add(MessageDim);
        MessageInt32MultiArray->layout.data_offset = 0;

        MessageInt32MultiArray->data.Add(rear_left);
        MessageInt32MultiArray->data.Add(rear_right);
        MessageInt32MultiArray->data.Add(front_left);
        MessageInt32MultiArray->data.Add(front_right);

        topic_wheels->Publish(MessageInt32MultiArray);
    }
}

void AMyActor::PublishOdometry(FVector position, FQuat orientation, FVector linear_velocity, FVector angular_velocity) {
    if (paused) {
        return;
    }

    if (topic_odom->IsAdvertising()) {
        ROSMessages::std_msgs::Header MessageHeader(odom_seq++, FROSTime::Now(), FString(TEXT("odom")));

        TArray<double> covariance;
        covariance.Init(0.0, 36);

        // Initial values from Gazebo sim
        covariance[0] = 0.001;
        covariance[7] = 0.001;
        covariance[14] = 0.001;
        covariance[21] = 0.001;
        covariance[28] = 0.001;
        covariance[35] = 0.03;

        ROSMessages::geometry_msgs::Pose MessagePose;
        MessagePose.position = position;
        MessagePose.orientation = orientation;

        ROSMessages::geometry_msgs::PoseWithCovariance MessagePoseWithCovariance;
        MessagePoseWithCovariance.pose = MessagePose;
        MessagePoseWithCovariance.covariance = covariance;

        ROSMessages::geometry_msgs::Twist MessageTwist;
        MessageTwist.linear = linear_velocity;
        MessageTwist.angular = angular_velocity;

        ROSMessages::geometry_msgs::TwistWithCovariance MessageTwistWithCovariance;
        MessageTwistWithCovariance.twist = MessageTwist;
        MessageTwistWithCovariance.covariance = covariance;

        TSharedPtr<ROSMessages::nav_msgs::Odometry> MessageOdometry(new ROSMessages::nav_msgs::Odometry());
        MessageOdometry->header = MessageHeader;
        MessageOdometry->child_frame_id = FString(TEXT("robot_footprint"));
        MessageOdometry->pose = MessagePoseWithCovariance;
        MessageOdometry->twist = MessageTwistWithCovariance;

        topic_odom->Publish(MessageOdometry);
    }
}

void AMyActor::PublishIMU(FQuat orientation, FVector angular_velocity, FVector linear_acceleration) {
    if (paused) {
        return;
    }

    if (topic_imu->IsAdvertising()) {
        ROSMessages::std_msgs::Header MessageHeader(imu_seq++, FROSTime::Now(), FString(TEXT("base_link")));

        TArray<double> covariance;
        covariance.Init(0.0, 9);

        ROSMessages::geometry_msgs::Quaternion MessageOrientation(orientation);
        ROSMessages::geometry_msgs::Vector3 MessageAngularVelocity(angular_velocity);
        ROSMessages::geometry_msgs::Vector3 MessageLinearAcceleration(linear_acceleration);

        TSharedPtr<ROSMessages::sensor_msgs::Imu> MessageIMU(new ROSMessages::sensor_msgs::Imu());
        MessageIMU->header = MessageHeader;
        MessageIMU->orientation = MessageOrientation;
        MessageIMU->angular_velocity = MessageAngularVelocity;
        MessageIMU->linear_acceleration = MessageLinearAcceleration;

        MessageIMU->orientation_covariance = covariance;
        MessageIMU->angular_velocity_covariance = covariance;
        MessageIMU->linear_acceleration_covariance = covariance;

        topic_imu->Publish(MessageIMU);
    }
}
