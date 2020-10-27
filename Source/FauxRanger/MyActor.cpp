#include "MyActor.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"
#include "ROSIntegration/Public/geometry_msgs/Twist.h"
#include "ROSIntegration/Public/std_msgs/Float32.h"
#include "ROSIntegration/Public/sensor_msgs/Imu.h"

AMyActor::AMyActor() {
  PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay() {
  Super::BeginPlay();

  if (EnableROS) {
    // Initialize a topic
    CmdVel = NewObject<UTopic>(UTopic::StaticClass());
    WheelVelocityFR = NewObject<UTopic>(UTopic::StaticClass());
    WheelVelocityFL = NewObject<UTopic>(UTopic::StaticClass());
    WheelVelocityRR = NewObject<UTopic>(UTopic::StaticClass());
    WheelVelocityRL = NewObject<UTopic>(UTopic::StaticClass());
    IMU = NewObject<UTopic>(UTopic::StaticClass());

    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());

    if (rosinst) {
      CmdVel->Init(rosinst->ROSIntegrationCore, TEXT("/cmd_vel"), TEXT("geometry_msgs/Twist"));
      WheelVelocityFR->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/front/right/velocity"), TEXT("std_msgs/Float32"));
      WheelVelocityFL->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/front/left/velocity"), TEXT("std_msgs/Float32"));
      WheelVelocityRR->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/rear/right/velocity"), TEXT("std_msgs/Float32"));
      WheelVelocityRL->Init(rosinst->ROSIntegrationCore, TEXT("/wheels/rear/left/velocity"), TEXT("std_msgs/Float32"));
      IMU->Init(rosinst->ROSIntegrationCore, TEXT("/imu"), TEXT("sensor_msgs/Imu"));

      WheelVelocityFR->Advertise();
      WheelVelocityFL->Advertise();
      WheelVelocityRR->Advertise();
      WheelVelocityRL->Advertise();
      IMU->Advertise();

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

    } else {
      UE_LOG(LogTemp, Warning, TEXT("Setting up ROS instance failed!"))
    }

  } else {
    UE_LOG(LogTemp, Warning, TEXT("ROS is not enabled on Rover actor!"));
  }
}

void AMyActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

void AMyActor::PublishWheelVelocities(float velocityFR, float velocityFL, float velocityRR, float velocityRL) {
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageFR(new ROSMessages::std_msgs::Float32(velocityFR));
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageFL(new ROSMessages::std_msgs::Float32(velocityFL));
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageRR(new ROSMessages::std_msgs::Float32(velocityRR));
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageRL(new ROSMessages::std_msgs::Float32(velocityRL));

  WheelVelocityFR->Publish(MessageFR);
  WheelVelocityFL->Publish(MessageFL);
  WheelVelocityRR->Publish(MessageRR);
  WheelVelocityRL->Publish(MessageRL);
}

void AMyActor::PublishIMU() {
  // TODO
}

