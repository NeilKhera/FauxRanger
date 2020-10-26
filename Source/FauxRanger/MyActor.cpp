#include "MyActor.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"
#include "ROSIntegration/Public/geometry_msgs/Twist.h"
#include "ROSIntegration/Public/std_msgs/Float32.h"

AMyActor::AMyActor() {
  PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay() {
  Super::BeginPlay();

  if (EnableROS) {
    // Initialize a topic
    CmdVel = NewObject<UTopic>(UTopic::StaticClass());
    TickerFR = NewObject<UTopic>(UTopic::StaticClass());
    TickerFL = NewObject<UTopic>(UTopic::StaticClass());
    TickerRR = NewObject<UTopic>(UTopic::StaticClass());
    TickerRL = NewObject<UTopic>(UTopic::StaticClass());

    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());

    CmdVel->Init(rosinst->ROSIntegrationCore, TEXT("/cmd_vel"), TEXT("geometry_msgs/Twist"));
    TickerFR->Init(rosinst->ROSIntegrationCore, TEXT("/ticks/FR"), TEXT("std_msgs/Float32"));
    TickerFL->Init(rosinst->ROSIntegrationCore, TEXT("/ticks/FL"), TEXT("std_msgs/Float32"));
    TickerRR->Init(rosinst->ROSIntegrationCore, TEXT("/ticks/RR"), TEXT("std_msgs/Float32"));
    TickerRL->Init(rosinst->ROSIntegrationCore, TEXT("/ticks/RL"), TEXT("std_msgs/Float32"));

    TickerFR->Advertise();
    TickerFL->Advertise();
    TickerRR->Advertise();
    TickerRL->Advertise();

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
}

void AMyActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

void AMyActor::PublishTicks(float ticksFR, float ticksFL, float ticksRR, float ticksRL) {
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageFR(new ROSMessages::std_msgs::Float32(ticksFR));
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageFL(new ROSMessages::std_msgs::Float32(ticksFL));
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageRR(new ROSMessages::std_msgs::Float32(ticksRR));
  TSharedPtr<ROSMessages::std_msgs::Float32> MessageRL(new ROSMessages::std_msgs::Float32(ticksRL));

  TickerFR->Publish(MessageFR);
  TickerFL->Publish(MessageFL);
  TickerRR->Publish(MessageRR);
  TickerRL->Publish(MessageRL);
}

