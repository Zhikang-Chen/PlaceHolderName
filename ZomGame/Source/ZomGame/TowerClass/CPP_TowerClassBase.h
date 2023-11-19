// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TowerStat.h"
#include "CPP_TowerClassBase.generated.h"

UCLASS()
class ZOMGAME_API ACPP_TowerClassBase : public AActor
{
	GENERATED_BODY()

	// Delegate use for the functionality
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAdditionalAttack, AActor*, Tower);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBasicAttack, AActor*, Tower);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, Tower);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKill, AActor*, Target);


#pragma region Variables 
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegate|Attack")
	FOnAdditionalAttack OnAdditionalAttack;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegate|Attack")
	FOnBasicAttack OnBasicAttack;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegate")
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegate")
	FOnKill OnKill;

	// Modifiers

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	FTowerStats StatModifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	FTowerStats StatMultiplier;

private:
protected:

	// Components
	//UPROPERTY(BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	//	class UCPP_ActorHealthComponent* ActorHealth;

	//UPROPERTY(BlueprintReadOnly, Category = Highlight, meta = (AllowPrivateAccess = "true"))
	//	class UCPP_ActorHighlightComponent* ActorHighlight;

	//UPROPERTY(BlueprintReadOnly)
	//	class UWidgetComponent* InteractionWidget;

	UPROPERTY(BlueprintReadOnly)
		class USceneComponent* Root;

	//UPROPERTY(BlueprintReadOnly)
	//	class UCPP_EntityStatusEffectComponent* StatusEffect;

	UPROPERTY(BlueprintReadWrite)
		class USceneComponent* ProjectileSpawnPoint;

	// Variables
	// -------------------------------------------------------------------------------------------------------------
	// Base stat

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
		FTowerStats StatBase;

	// This is here temporary
	// It's just for the sake of accessing the Modifiers
	//UPROPERTY(VisibleAnywhere)
	//class UCPP_TowerBuildModeComponent* BuildComponent;

	// -------------------------------------------------------------------------------------------------------------
	// Attack

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCanAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
		FTimerHandle AttackingTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
		float AttackDelay;

	// Targeting
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
		float TargetDetectionUpdateRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Targeting")
		bool bCanTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
		FTimerHandle TargetingTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
		TArray<AActor*> AllTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Targeting")
		AActor* CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
		FName TargetProfileName;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	//	TSubclassOf<class ACPP_PPProjectileBase> MainProjectile;

#pragma endregion
#pragma region Functions


public:
	// Sets default values for this actor's properties
	ACPP_TowerClassBase();

private:
	UFUNCTION()
	void OnTowerDeath();

	UFUNCTION()
	void OnTowerKill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Targeting detection for cpp to override
	virtual void TargetDetection();

	// Targeting detection for blueprint implementation
	// This will override any cpp
	UFUNCTION(BlueprintNativeEvent, Category = "Targeting", meta = (DisplayName = "TargetDetection"))
		void ReceiveTargetDetection();

	// Attack for cpp to override
	virtual void AttackTarget();

	// Attack for blueprint implementation
	// This will override any cpp
	UFUNCTION(BlueprintNativeEvent, Category = "Attack", meta = (DisplayName = "AttackTarget"))
		void ReceiveAttackTarget();

	// Becareful when calling this from other class
	// Chould create a loop
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void InvokeAllAttacks();

public:	
	UFUNCTION(BlueprintCallable, Category = "Attack")
		void InvokeBasicAttack();

	UFUNCTION(BlueprintCallable)
		void Selection();

	UFUNCTION(BlueprintCallable)
		void Deselection();

	//UFUNCTION(BlueprintCallable)
	//	class UCPP_BaseStatusEffect* AddStatus(TSubclassOf<UCPP_BaseTowerStatusEffect> Effect, float Time = 0.f);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool HasLOS(AActor* Target);

// ---------------------------------------------------------------
// getter for the stats

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetTowerMaxHealth();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetAttackSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetDamage();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetTargetingRange();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetDeployTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetPierce();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetAggro();

	//UFUNCTION(BlueprintCallable, BlueprintPure)
	//TSubclassOf<class ACPP_PPProjectileBase> GetMainProjectileClass();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AActor* GetCurrentTarget();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		const TArray<AActor*> GetAllTargets();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		const USceneComponent* GetProjectileSpawnPoint();
	// Friend class
	// Not clean but it works and save some time
	//friend class UCPP_TowerBuildModeComponent;
	//friend class UCPP_UpgradeFunctionality;
#pragma endregion
};
