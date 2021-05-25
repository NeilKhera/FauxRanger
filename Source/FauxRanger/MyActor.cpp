#include "MyActor.h"
#include "ROSTime.h"

#include "FauxRangerBlueprintLibrary.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"

#include "ROSIntegration/Public/sensor_msgs/Imu.h"
#include "ROSIntegration/Public/nav_msgs/Odometry.h"
#include "ROSIntegration/Public/rosgraph_msgs/Clock.h"
#include "ROSIntegration/Public/rasm/RASM_GOAL_MSG.h"

#include "ROSIntegration/Public/std_msgs/Header.h"
#include "ROSIntegration/Public/std_msgs/Int32MultiArray.h"

#include "ROSIntegration/Public/geometry_msgs/Vector3.h"
#include "ROSIntegration/Public/geometry_msgs/Pose.h"
#include "ROSIntegration/Public/geometry_msgs/Twist.h"
#include "ROSIntegration/Public/geometry_msgs/PoseWithCovariance.h"
#include "ROSIntegration/Public/geometry_msgs/TwistWithCovariance.h"


AMyActor::AMyActor() {
    PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay() {
    Super::BeginPlay();
}

void AMyActor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (update_sun) {
        update_sun = false;
        this->VectorEvent(sun_vector);
    }

    if (spawn_waypoint) {
        spawn_waypoint = false;
        this->SpawnEvent(waypoint_location, waypoint_rotation, waypoint_scale);
    }
}

void AMyActor::Pause(const bool pause) {
    paused = pause;
}

void AMyActor::InitializeTopics() {
    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());

    if (rosinst && rosinst->bConnectToROS) {
        // Initialize a topic
        topic_sun_seeker = NewObject<UTopic>(UTopic::StaticClass());
        topic_goal = NewObject<UTopic>(UTopic::StaticClass());
        topic_cmd_vel = NewObject<UTopic>(UTopic::StaticClass());
        topic_wheels = NewObject<UTopic>(UTopic::StaticClass());
        topic_odom = NewObject<UTopic>(UTopic::StaticClass());
        topic_IMU = NewObject<UTopic>(UTopic::StaticClass());

        topic_sun_seeker->Init(rosinst->ROSIntegrationCore, TEXT("/sun_seeker/vector"), TEXT("geometry_msgs/Vector3"));
        topic_goal->Init(rosinst->ROSIntegrationCore, TEXT("/rover_executive/goal_command"), TEXT("rasm/RASM_GOAL_MSG"));
        topic_cmd_vel->Init(rosinst->ROSIntegrationCore, TEXT("/moon_ranger_velocity_controller/cmd_vel"), TEXT("geometry_msgs/Twist"));
        topic_wheels->Init(rosinst->ROSIntegrationCore, TEXT("/wheels"), TEXT("std_msgs/Int32MultiArray"));
        topic_odom->Init(rosinst->ROSIntegrationCore, TEXT("/moon_ranger_velocity_controller/odom"), TEXT("nav_msgs/Odometry"));
        topic_IMU->Init(rosinst->ROSIntegrationCore, TEXT("/imu/data"), TEXT("sensor_msgs/Imu"));

        topic_wheels->Advertise();
        topic_odom->Advertise();
        topic_IMU->Advertise();

        std::function<void(TSharedPtr<FROSBaseMsg>)> SunSeekerCallback = [this](TSharedPtr<FROSBaseMsg> msg) -> void {
            auto Concrete = StaticCastSharedPtr<ROSMessages::geometry_msgs::Vector3>(msg);
            if (Concrete.IsValid()) {
                this->sun_vector = FVector(Concrete->x, Concrete->y, Concrete->z);
                this->update_sun = true;
            }
        };

        std::function<void(TSharedPtr<FROSBaseMsg>)> GoalCallback = [this](TSharedPtr<FROSBaseMsg> msg) -> void {
            auto Concrete = StaticCastSharedPtr<ROSMessages::rasm::RASM_GOAL_MSG>(msg);
            if (Concrete.IsValid()) {
                this->waypoint_location = FVector(Concrete->origin_x_meters * 100, - Concrete->origin_y_meters * 100, UFauxRangerBlueprintLibrary::GetSurface(this->GetWorld(), FVector2D(Concrete->origin_x_meters * 100, - Concrete->origin_y_meters * 100), false));
                this->waypoint_rotation = FRotator(0.0f, 0.0f, Concrete->orientation_radians);
                this->waypoint_scale = FVector(Concrete->length_meters, Concrete->width_meters, 1.0f);

                this->spawn_waypoint = true;
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
        topic_sun_seeker->Subscribe(SunSeekerCallback);
        topic_goal->Subscribe(GoalCallback);
        topic_cmd_vel->Subscribe(CmdVelCallback);

        if (rosinst->bSimulateTime) {
            topic_clock = NewObject<UTopic>(UTopic::StaticClass());
            topic_clock->Init(rosinst->ROSIntegrationCore, TEXT("/clock"), TEXT("rosgraph_msgs/Clock"));
            topic_clock->Advertise();
        }

    } else {
        UE_LOG(LogTemp, Warning, TEXT("Setting up ROS instance failed!"));
    }
}

void AMyActor::PublishWheelData(int32 rear_left, int32 rear_right, int32 front_left, int32 front_right) {
    if (topic_wheels && topic_wheels->IsAdvertising()) {
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
    if (topic_odom && topic_odom->IsAdvertising()) {
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

    topic_IMU->Publish(MessageIMU);
}

void AMyActor::PublishClock() {
    if (Cast<UROSIntegrationGameInstance>(GetGameInstance())->bSimulateTime && topic_clock && topic_clock->IsAdvertising()) {
        float current_time = GetWorld()->GetTimeSeconds();

        unsigned long seconds = (unsigned long)current_time;
        unsigned long nanoseconds = (unsigned long)((current_time - seconds) * 1000000000ul);

        TSharedPtr<ROSMessages::rosgraph_msgs::Clock> ClockMessage(new ROSMessages::rosgraph_msgs::Clock(FROSTime(seconds, nanoseconds)));
        topic_clock->Publish(ClockMessage);
    }
}