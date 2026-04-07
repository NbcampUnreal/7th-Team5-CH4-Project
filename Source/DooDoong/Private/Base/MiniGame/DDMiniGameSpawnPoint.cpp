#include "Base/MiniGame/DDMiniGameSpawnPoint.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"

ADDMiniGameSpawnPoint::ADDMiniGameSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스폰 포인트는 배치 전용 액터라 일단 Scene 컴포넌트만 설정
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	// 방향 확인을 위해 ArrowComponent 설정
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(SceneRoot);
}
