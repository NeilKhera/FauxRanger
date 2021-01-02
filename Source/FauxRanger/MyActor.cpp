#include "MyActor.h"
#include "ROSTime.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"

#include "ROSIntegration/Public/sensor_msgs/Imu.h"
#include "ROSIntegration/Public/nav_msgs/Odometry.h"

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

  // Initialize a topic
  topic_cmd_vel = NewObject<UTopic>(UTopic::StaticClass());
  topic_wheels = NewObject<UTopic>(UTopic::StaticClass());
  topic_odom = NewObject<UTopic>(UTopic::StaticClass());
  topic_imu = NewObject<UTopic>(UTopic::StaticClass());

  // TESTING
  InitializeTopics();
}

void AMyActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

void AMyActor::Pause(const bool pause) {
  paused = pause;
}

void AMyActor::InitializeTopics() {
    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());

    if (rosinst && rosinst->bConnectToROS) {
        topic_cmd_vel->Init(rosinst->ROSIntegrationCore, TEXT("/moon_ranger_velocity_controller/cmd_vel"), TEXT("geometry_msgs/Twist"));
        topic_wheels->Init(rosinst->ROSIntegrationCore, TEXT("/wheels"), TEXT("std_msgs/Int32MultiArray"));
        topic_odom->Init(rosinst->ROSIntegrationCore, TEXT("/moon_ranger_velocity_controller/odom"), TEXT("nav_msgs/Odometry"));
        topic_imu->Init(rosinst->ROSIntegrationCore, TEXT("/imu/data"), TEXT("sensor_msgs/Imu"));

        topic_wheels->Advertise();
        topic_odom->Advertise();
        topic_imu->Advertise();

        // Create a std::function callback object
        std::function<void(TSharedPtr<FROSBaseMsg>)> SubscribeCallback = [this](TSharedPtr<FROSBaseMsg> msg) -> void {
            auto Concrete = StaticCastSharedPtr<ROSMessages::geometry_msgs::Twist>(msg);
            if (Concrete.IsValid()) {
                this->TeleopEvent(Concrete->linear.x, Concrete->angular.z);
            }
            return;
        };

        // Subscribe to the topic
        topic_cmd_vel->Subscribe(SubscribeCallback);
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
        TSharedPtr<ROSMessages::std_msgs::Int32MultiArray> MessageInt32MultiArray(new ROSMessages::std_msgs::Int32MultiArray());

        MessageInt32MultiArray->data.Add(rear_left);
        MessageInt32MultiArray->data.Add(rear_right);
        MessageInt32MultiArray->data.Add(front_left);
        MessageInt32MultiArray->data.Add(front_right);

        topic_wheels->Publish(MessageInt32MultiArray);
    }
}

void AMyActor::PublishOdometry(FVector position, FQuat orientation) {
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

        ROSMessages::geometry_msgs::TwistWithCovariance MessageTwistWithCovariance;
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
