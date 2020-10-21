#include "MyActor.h"

#include "ROSIntegration/Classes/RI/Topic.h"
#include "ROSIntegration/Classes/ROSIntegrationGameInstance.h"
#include "ROSIntegration/Public/geometry_msgs/Twist.h"

AMyActor::AMyActor() {
  PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay() {
  Super::BeginPlay();

  if (EnableROS) {
    // Initialize a topic
    CmdVel = NewObject<UTopic>(UTopic::StaticClass());
    UROSIntegrationGameInstance* rosinst = Cast<UROSIntegrationGameInstance>(GetGameInstance());
    CmdVel->Init(rosinst->ROSIntegrationCore, TEXT("/cmd_vel"), TEXT("geometry_msgs/Twist"));

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

