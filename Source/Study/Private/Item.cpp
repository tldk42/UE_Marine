#include "Item.h"

#include "Marine.h"

AItem::AItem()
	: ItemName("Default"),
	  ItemCount(0),
	  ItemRarity(EItemRarity::EIR_Common)
{
	ActiveStars.Init(false, 5);
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(RootComponent);
//  CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	PickupWidget->SetupAttachment(RootComponent);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(RootComponent);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	// 우선 알림 위젯을 가리고 캐릭터와 오버랩되면 다시 나타낸다.
	PickupWidget->SetVisibility(false);
	SetActiveStar();

	// Setup overlap for areasphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::SphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::SphereEndOverlap);
}

void AItem::SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Actor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Actor)
	{
		AMarine* Player = Cast<AMarine>(Actor);
		if (Player)
		{
			Player->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Actor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex)
{
	if (Actor)
	{
		AMarine* Player = Cast<AMarine>(Actor);
		if (Player)
		{
			Player->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStar()
{
	// for (auto& bActiveStar : ActiveStars)
	// {
	// 	bActiveStar = false;
	// }

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[0] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[0] = true;
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_UnCommon:
		ActiveStars[0] = true;
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[0] = true;
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;

		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[0] = true;
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_MAX:
		break;
	}
}


void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::HideWidget() const
{
	PickupWidget->SetVisibility(false);
}

void AItem::ShowWidget() const
{
	PickupWidget->SetVisibility(true);
}
