#include "MiniGames/Catch/Actors/DiamondActor.h"
#include "GameFramework/Character.h"
#include "MiniGames/Catch/GameMode/DDCatchGameMode.h"


ADiamondActor::ADiamondActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	Mesh->SetGenerateOverlapEvents(true);
}

void ADiamondActor::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ADiamondActor::OnOverlap);
}

void ADiamondActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (!IsInUse()) return;
	
	SetInUse(false); // 풀로 반환
	
	// 플레이어 캐릭터 체크
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character) return;

	// PlayerState 가져오기
	APlayerState* PS = Character->GetPlayerState();
	if (!PS) return;

	// GameMode 가져오기
	ADDCatchGameMode* GM = Cast<ADDCatchGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	// 점수 추가
	GM->AddScore(PS, 1);
}
