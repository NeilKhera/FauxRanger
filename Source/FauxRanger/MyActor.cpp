#include "MyActor.h"
#include "ROSTime.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"

#include "ROSIntegration/Public/std_msgs/Header.h"
#include "ROSIntegration/Public/std_msgs/Float32.h"
#include "ROSIntegration/Public/nav_msgs/Odometry.h"
#include "ROSIntegration/Public/sensor_msgs/Imu.h"

#include "ROSIntegration/Public/geometry_msgs/Pose.h"
#include "ROSIntegration/Public/geometry_msgs/Twist.h"
#include "ROSIntegration/Public/geometry_msgs/PoseWithCovariance.h"
#include "ROSIntegration/Public/geometry_msgs/TwistWithCovariance.h"

AMyActor::AMyActor() {
  PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay() {
  Super::BeginPlay();

  Paused = false;

  IMUSeq = 0;
  OdometrySeq = 0;

  // Initialize a topic
  CmdVel = NewObject<UTopic>(UTopic::StaticClass());
  WheelVelocityFR = NewObject<UTopic>(UTopic::StaticClass());
  WheelVelocityFL = NewObject<UTopic>(UTopic::StaticClass());
  WheelVelocityRR = NewObject<UTopic>(UTopic::StaticClass());
  WheelVelocityRL = NewObject<UTopic>(UTopic::StaticClass());
  IMU = NewObject<UTopic>(UTopic::StaticClass());
  Odometry = NewObject<UTopic>(UTopic::StaticClass());
}

void AMyActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

void AMyActor::Pause(const bool _Pause) {
  Paused = _Pause;
}

void AMyActor::InitializeTopics() {
    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());

    if (rosinst && rosinst->bConnectToROS) {
        CmdVel->Init(rosinst->ROSIntegrationCore, TEXT("/cmd_vel"), TEXT("geometry_msgs/Twist"));
        WheelVelocityFR->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/front/right/velocity"), TEXT("std_msgs/Float32"));
        WheelVelocityFL->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/front/left/velocity"), TEXT("std_msgs/Float32"));
        WheelVelocityRR->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/rear/right/velocity"), TEXT("std_msgs/Float32"));
        WheelVelocityRL->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/rear/left/velocity"), TEXT("std_msgs/Float32"));
        IMU->Init(rosinst->ROSIntegrationCore, TEXT("/imu/data"), TEXT("sensor_msgs/Imu"));
        Odometry->Init(rosinst->ROSIntegrationCore, TEXT("/odom"), TEXT("nav_msgs/Odometry"));

        WheelVelocityFR->Advertise();
        WheelVelocityFL->Advertise();
        WheelVelocityRR->Advertise();
        WheelVelocityRL->Advertise();
        IMU->Advertise();
        Odometry->Advertise();

        // Create a std::function callback object
        std::function<void(TSharedPtr<FROSBaseMsg>)> SubscribeCallback = [this](TSharedPtr<FROSBaseMsg> msg) -> void {
            auto Concrete = StaticCastSharedPtr<ROSMessages::geometry_msgs::Twist>(msg);
            if (Concrete.IsValid()) {
                this->TeleopEvent(Concrete->linear.x, Concrete->angular.z);
            }
            return;
        };

        // Subscribe to the topic
        CmdVel->Subscribe(SubscribeCallback);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Setting up ROS instance failed!"));
    }
}

void AMyActor::PublishWheelVelocities(float velocityFR, float velocityFL, float velocityRR, float velocityRL) {
    if (Paused) {
        return;
    }

    if (WheelVelocityFL->IsAdvertising() && WheelVelocityFR->IsAdvertising() && WheelVelocityRL->IsAdvertising() && WheelVelocityRR->IsAdvertising()) {
        TSharedPtr<ROSMessages::std_msgs::Float32> MessageFR(new ROSMessages::std_msgs::Float32(velocityFR));
        TSharedPtr<ROSMessages::std_msgs::Float32> MessageFL(new ROSMessages::std_msgs::Float32(velocityFL));
        TSharedPtr<ROSMessages::std_msgs::Float32> MessageRR(new ROSMessages::std_msgs::Float32(velocityRR));
        TSharedPtr<ROSMessages::std_msgs::Float32> MessageRL(new ROSMessages::std_msgs::Float32(velocityRL));

        WheelVelocityFR->Publish(MessageFR);
        WheelVelocityFL->Publish(MessageFL);
        WheelVelocityRR->Publish(MessageRR);
        WheelVelocityRL->Publish(MessageRL);
    }
}

void AMyActor::PublishIMU(FQuat orientation, FVector angular_velocity, FVector linear_acceleration) {
    if (Paused) {
        return;
    }

    if (IMU->IsAdvertising()) {
        ROSMessages::std_msgs::Header MessageHeader(IMUSeq++, FROSTime::Now(), FString(TEXT("base_link")));

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

        IMU->Publish(MessageIMU);
    }
}

void AMyActor::PublishOdometry(FVector position, FQuat orientation) {
    if (Paused) {
        return;
    }

    if (Odometry->IsAdvertising()) {
        ROSMessages::std_msgs::Header MessageHeader(IMUSeq++, FROSTime::Now(), FString(TEXT("base_link")));

        TArray<double> covariance;
        covariance.Init(0.0, 36);

        ROSMessages::geometry_msgs::Pose MessagePose;
        MessagePose.position = position;
        MessagePose.orientation = orientation;

        ROSMessages::geometry_msgs::PoseWithCovariance MessagePoseWithCovariance;
        MessagePoseWithCovariance.pose = MessagePose;
        MessagePoseWithCovariance.covariance = covariance;

        ROSMessages::geometry_msgs::TwistWithCovariance MessageTwistWithCovariance;
        MessageTwistWithCovariance.covariance = covariance;

        TSharedPtr<ROSMessages::nav_msgs::Odometry> MessageOdometry(new ROSMessages::nav_msgs::Odometry());
        MessageOdometry->header = MessageHeader;
        MessageOdometry->child_frame_id = FString(TEXT("odom"));
        MessageOdometry->pose = MessagePoseWithCovariance;
        MessageOdometry->twist = MessageTwistWithCovariance;

        Odometry->Publish(MessageOdometry);
    }
}
