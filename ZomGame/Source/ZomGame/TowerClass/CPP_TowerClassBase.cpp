// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_TowerClassBase.h"
#include "Components/WidgetComponent.h"
//#include "TargetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectGlobals.h"
//#include "EventDispatcherManager.h"
#include <UObject/ConstructorHelpers.h>
#include "Components/SceneComponent.h"
//#include "CPP_TowerBuildModeComponent.h"
//#include "Project_Primis/General Components/CPP_ActorHealthComponent.h"
//#include "Project_Primis/General Components/CPP_ActorHighlightComponent.h"
//#include "Project_Primis/General Actors/CPP_PPProjectileBase.h"
//#include "Project_Primis/User Interfaces/CPP_TowerInteractionWidget.h"
//#include "Project_Primis/General Utilities/CPP_DebugUtilities.h"
//#include "Project_Primis/Status Effect System/Tower Effect/CPP_BaseTowerStatusEffect.h"
//#include "Project_Primis/Status Effect System/CPP_EntityStatusEffectComponent.h"
//#include "TowerStat.h"


// Sets default values
ACPP_TowerClassBase::ACPP_TowerClassBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setting the stats
	StatBase.Health = 10.f;
	StatBase.AttackSpeed = 1.0f;
	StatBase.Damage = 1.0f;
	StatBase.TargetingRange = 1000.0f;
	StatBase.DeployTime = 1.0f;
	StatBase.Pierce = 1.0f;
	StatBase.Aggro = 1.0f;

	// Attack and targeting
	bCanAttack = true;
	AttackDelay = 1.0f;
	TargetDetectionUpdateRate = 0.1f;
	bCanTarget = true;
	TargetProfileName = "TowerTargeting";

	// A new root because the interaction widget keep overriding the root.
	Root = CreateDefaultSubobject< USceneComponent>("Root");
	this->SetRootComponent(RootComponent);

	//ActorHealth = CreateDefaultSubobject<UCPP_ActorHealthComponent>(TEXT("ActorHealth"));

	//ActorHighlight = CreateDefaultSubobject<UCPP_ActorHighlightComponent>(TEXT("ActorHighlight"));

	// The path is hardcoded in. Change this to soft path later.
	//static ConstructorHelpers::FClassFinder<UCPP_TowerInteractionWidget> widgetClass(TEXT("/Game/UI/WBP_TowerInteraction"));
	//InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	//InteractionWidget->SetupAttachment(Root);
	//InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//InteractionWidget->SetVisibility(false);
	//InteractionWidget->SetWidgetClass(widgetClass.Class);

	//StatusEffect = CreateDefaultSubobject<UCPP_EntityStatusEffectComponent>(TEXT("StatusEffect"));
}

void ACPP_TowerClassBase::OnTowerDeath()
{
	OnDeath.Broadcast(this);
}

void ACPP_TowerClassBase::OnTowerKill()
{
	OnKill.Broadcast(this);
}

// Called when the game starts or when spawned
void ACPP_TowerClassBase::BeginPlay()
{
	Super::BeginPlay();
	//ActorHealth->SetMaxHealth(GetTowerMaxHealth());
	//ActorHealth->SetCurrentHealth(GetTowerMaxHealth());
	//ActorHealth->OnDeath.AddDynamic(this, &ACPP_TowerClassBase::OnTowerDeath);

	//FSimpleEventDelegate eventDelegate;
	//eventDelegate.BindUFunction(this, TEXT("OnTowerKill"));
	//UEventManager::RegisterSimpleEventName(this, TEXT("EI_Kill"), eventDelegate);

	GetWorldTimerManager().SetTimer(TargetingTimer, this, &ACPP_TowerClassBase::ReceiveTargetDetection, TargetDetectionUpdateRate, true);
}

void ACPP_TowerClassBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	//StatusEffect->DeinitializeAll();

	OnAdditionalAttack.RemoveAll(this);
	OnBasicAttack.RemoveAll(this);
	OnDeath.RemoveAll(this);
	OnKill.RemoveAll(this);
}

// Called every frame
void ACPP_TowerClassBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Functions design to be override it's child

void ACPP_TowerClassBase::TargetDetection()
{
	auto location = this->GetActorLocation();
	TArray<AActor*> ignore;
	TArray<FHitResult> result;
	AllTarget.Empty();

	UKismetSystemLibrary::SphereTraceMultiByProfile(this, location, location, GetTargetingRange(), TargetProfileName,
		false, ignore, EDrawDebugTrace::ForDuration, result, true, FLinearColor::Red, FLinearColor::Green, 0.2);

	
	// Maybe make this component thing?
	for (int i = 0; i < result.Num(); i++)
	{
		if (!AllTarget.Contains(result[i].GetActor()))
		{
				AllTarget.Add(result[i].GetActor());
			//if (HasLOS(result[i].GetActor()))
			//{
			//}
		}
	}


	if (result.Num() > 0)
	{
		if(!AttackingTimer.IsValid())
			GetWorldTimerManager().SetTimer(AttackingTimer, this, &ACPP_TowerClassBase::InvokeAllAttacks, AttackDelay / GetAttackSpeed(), true);
		return;
	}

	GetWorldTimerManager().ClearTimer(AttackingTimer);
	AttackingTimer.Invalidate();
}

void ACPP_TowerClassBase::ReceiveTargetDetection_Implementation()
{
	TargetDetection();
}

void ACPP_TowerClassBase::AttackTarget()
{

}

void ACPP_TowerClassBase::ReceiveAttackTarget_Implementation()
{
	AttackTarget();
}


// 

void ACPP_TowerClassBase::InvokeAllAttacks()
{
	if (bCanAttack)
	{
		InvokeBasicAttack();
		OnAdditionalAttack.Broadcast(this);
		//ReceiveAttackTarget();
	}
}

void ACPP_TowerClassBase::InvokeBasicAttack()
{
	ReceiveAttackTarget();
	OnBasicAttack.Broadcast(this);
}

void ACPP_TowerClassBase::Selection()
{
	//InteractionWidget->SetVisibility(true);
	//ActorHighlight->Highlight();
}

void ACPP_TowerClassBase::Deselection()
{
	//InteractionWidget->SetVisibility(false);
	//ActorHighlight->Unhighlight();
}

//UCPP_BaseStatusEffect* ACPP_TowerClassBase::AddStatus(TSubclassOf<UCPP_BaseTowerStatusEffect> Effect, float Time)
//{
//	return StatusEffect->AddStatusEffect(Effect, Time);
//}

bool ACPP_TowerClassBase::HasLOS(AActor* Target)
{
	bool hidden = Target->IsHidden();
	if (!hidden)
	{
		FVector towerLocation;

		if (ProjectileSpawnPoint)
			towerLocation = ProjectileSpawnPoint->GetComponentLocation();
		else
			towerLocation = GetActorLocation();

		auto targetLocation = Target->GetActorLocation();

		FHitResult traceResult;
		TArray<AActor*> ignore;

		//ignore.Add(BuildComponent->GetOwner());

		ETraceTypeQuery traceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
		if (UKismetSystemLibrary::LineTraceSingle(this, towerLocation, targetLocation, traceChannel,
			false, ignore, EDrawDebugTrace::None, traceResult, true))
		{
			auto traceActor = traceResult.GetActor();
			if (traceActor == Target)
			{
				return true;
			}
		}
	}
	return false;
}

// -----------------------------------------------------------------------------------------------------
// Getter for the stats

float ACPP_TowerClassBase::GetTowerMaxHealth()
{
	//if (BuildComponent)
	//	return (StatBase.Health + BuildComponent->GobleTowerStatModifier.Health + StatModifier.Health) * 
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.Health + StatMultiplier.Health);
	return StatBase.Health;
}

float ACPP_TowerClassBase::GetAttackSpeed()
{
	//if(BuildComponent)
	//	return (StatBase.AttackSpeed + BuildComponent->GobleTowerStatModifier.AttackSpeed + StatModifier.AttackSpeed) *
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.AttackSpeed + StatMultiplier.AttackSpeed);
	return StatBase.AttackSpeed;
}

float ACPP_TowerClassBase::GetDamage()
{
	//if (BuildComponent)
	//	return (StatBase.Damage + BuildComponent->GobleTowerStatModifier.Damage + StatModifier.Damage) *
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.Damage + StatMultiplier.Damage);
	return StatBase.Damage;
}

float ACPP_TowerClassBase::GetTargetingRange()
{
	//if (BuildComponent)
	//	return (StatBase.TargetingRange + BuildComponent->GobleTowerStatModifier.TargetingRange + StatModifier.TargetingRange) *
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.TargetingRange + StatMultiplier.TargetingRange);
	return StatBase.TargetingRange;
}

float ACPP_TowerClassBase::GetDeployTime()
{
	//if (BuildComponent)
	//	return (StatBase.DeployTime + BuildComponent->GobleTowerStatModifier.DeployTime + StatModifier.DeployTime) *
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.DeployTime + StatMultiplier.DeployTime);
	return StatBase.DeployTime;
}

float ACPP_TowerClassBase::GetPierce()
{
	//if (BuildComponent)
	//	return (StatBase.Pierce + BuildComponent->GobleTowerStatModifier.Pierce + StatModifier.Pierce) *
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.Pierce + StatMultiplier.Pierce);
	return StatBase.Pierce;
}

float ACPP_TowerClassBase::GetAggro()
{
	//if (BuildComponent)
	//	return (StatBase.Aggro + BuildComponent->GobleTowerStatModifier.Aggro + StatModifier.Aggro) *
	//	(1.f + BuildComponent->GobleTowerStatMultiplier.Aggro + StatMultiplier.Aggro);
	return StatBase.Aggro;
}

//TSubclassOf<class ACPP_PPProjectileBase> ACPP_TowerClassBase::GetMainProjectileClass()
//{
//	return MainProjectile;
//}

AActor* ACPP_TowerClassBase::GetCurrentTarget()
{
	return CurrentTarget;
}

const TArray<AActor*> ACPP_TowerClassBase::GetAllTargets()
{
	return AllTarget;
}

const USceneComponent* ACPP_TowerClassBase::GetProjectileSpawnPoint()
{
	return ProjectileSpawnPoint;
}
