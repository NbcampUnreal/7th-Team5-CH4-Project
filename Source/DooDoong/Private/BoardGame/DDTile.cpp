#include "BoardGame/DDTile.h"

#include "AbilitySystemComponent.h"
#include "BoardGame/Character/DDBoardGameCharacter.h"
#include "Common/DDLogManager.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "System/DDGameplayTags.h"

ADDTile::ADDTile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Mesh
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(Root);

	// Stand Point
	StandPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StandPoint"));
	StandPoint->SetupAttachment(Root);
	
	bReplicates = true;
}

void ADDTile::BeginPlay()
{
	Super::BeginPlay();
}

FVector ADDTile::GetStandLocation(ACharacter* Character) const
{
	if (!StandPoint) return GetActorLocation();

	FVector Location = StandPoint->GetComponentLocation();

	if (Character)
	{
		float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Location.Z += HalfHeight;
	}

	return Location;
}

void ADDTile::LoadTileData()
{
	if (!HasAuthority()) return;
	if (!TileDataTable || TileRowName.IsNone())
	{
		LOG_CYS(Error, TEXT("[Tile]TileDataTable or RowName missing"));
		return;
	}

	FTileRowData* Row = TileDataTable->FindRow<FTileRowData>(TileRowName, TEXT(""));

	if (Row)
	{
		TileData = *Row;

		ApplyTileMaterial();
	}
	else
	{
		LOG_CYS(Error, TEXT("[Tile]Row not found: %s"), *TileRowName.ToString());
	}
}

void ADDTile::ResolveNextTiles(const TMap<FName, ADDTile*>& TileMap)
{
	// 기존 연결 초기화
	NextTiles.Empty();

	// DataTable에 NextTiles 없으면 종료
	if (TileData.NextTiles.Num() == 0)
	{
		return;
	}

	for (const FName& NextName : TileData.NextTiles)
	{
		if (NextName.IsNone())
		{
			continue;
		}

		ADDTile* FoundTile = TileMap.FindRef(NextName);

		if (FoundTile)
		{
			NextTiles.Add(FoundTile);
		}
		else
		{
			LOG_CYS(Error, TEXT("[Tile][%s] NextTile not found: %s"),
			        *TileRowName.ToString(),
			        *NextName.ToString());
		}
	}
}

void ADDTile::ApplyTileMaterial()
{
	if (!TileMesh) return;

	UMaterialInterface** FoundMat = TileMaterialMap.Find(TileData.TileType);

	if (FoundMat && *FoundMat)
	{
		TileMesh->SetMaterial(0, *FoundMat);
	}
	else
	{
		LOG_CYS(Warning, TEXT("[Tile] Material not found for TileType"));
	}
}

void ADDTile::OnRep_TileData()
{
	LOG_CYS(Warning, TEXT("[Tile] OnRep_TileData"));
	ApplyTileMaterial();
}

void ADDTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADDTile, TileData);
}

void ADDTile::OnCharacterArrived(ADDBoardGameCharacter* Character) const
{
	if (!Character) return;

	UAbilitySystemComponent* AbilitySystemComponent = Character->GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return;

	// 캐릭터 도착 타일 이벤트 처리
	FGameplayTag Tag;

	switch (TileData.TileType)
	{
	case ETileType::Goal:
		// 트로피 획득
		Tag = DDGameplayTags::Tile_Ability_Goal;
		LOG_CYS(Warning, TEXT("[Tile] OnCharacterArrived Goal!!"), *Tag.ToString());
		break;
		
	case ETileType::Coin:
		Tag = DDGameplayTags::Tile_Ability_Coin;
		LOG_CYS(Warning, TEXT("[Tile][%s] OnCharacterArrived"), *Tag.ToString());
		break;

	case ETileType::Item:
		Tag = DDGameplayTags::Tile_Ability_Item;
		LOG_CYS(Warning, TEXT("[Tile][%s] OnCharacterArrived"), *Tag.ToString());
		break;

	case ETileType::Move:
		Tag = DDGameplayTags::Tile_Ability_Move;
		LOG_CYS(Warning, TEXT("[Tile][%s] OnCharacterArrived"), *Tag.ToString());
		break;
	case ETileType::Damage:
		Tag = DDGameplayTags::Tile_Ability_Damage;
		LOG_CYS(Warning, TEXT("[Tile][%s] OnCharacterArrived"), *Tag.ToString());
		break;

	default:
		LOG_CYS(Warning, TEXT("[Tile] OnCharacterArrived No Event"));
		return;
	}
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(Tag));
}

bool ADDTile::IsGoal() const
{
	return (TileData.TileType==ETileType::Goal);
}

void ADDTile::OnConstruction(const FTransform& Transform)
{
	// TileRowName 자동 셋팅!!!
	Super::OnConstruction(Transform);

	FString Name = GetActorLabel();

	FString NumberPart;
	for (int32 i = Name.Len() - 1; i >= 0; i--)
	{
		if (FChar::IsDigit(Name[i]))
		{
			NumberPart.InsertAt(0, Name[i]);
		}
		else
		{
			break;
		}
	}

	if (!NumberPart.IsEmpty())
	{
		TileRowName = FName(*FString::Printf(TEXT("Tile%02d"), FCString::Atoi(*NumberPart)));
	}
}
