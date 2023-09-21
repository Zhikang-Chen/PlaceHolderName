// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 


#include "VertexPaintClothOverlapComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "VertexPaintDetectionInterface.h"

#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "ClothingAsset.h"


//-------------------------------------------------------

// Construct

UVertexPaintClothOverlapComponent::UVertexPaintClothOverlapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


//-------------------------------------------------------

// GetLifetimeReplicatedProps

void UVertexPaintClothOverlapComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UVertexPaintClothOverlapComponent, clothOverlapTracingEnabled);
}


//-------------------------------------------------------

// BeginPlay

void UVertexPaintClothOverlapComponent::BeginPlay() {

	Super::BeginPlay();


	parentSkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetAttachParent());

	if (IsValid(parentSkeletalMeshComponent) && clothOverlapTracingEnabled) {

		ActivateClothOverlapTracing();
	}

	else {

		DeActivateClothOverlapTracing();
	}
}


//-------------------------------------------------------

// Set Enable Cloth Overlap Tracing

void UVertexPaintClothOverlapComponent::SetClothOverlapTracingEnabled(bool enableClothTracing) {

	clothOverlapTracingEnabled = enableClothTracing;

	if (GetOwner()->HasAuthority())
		OnRep_ClothOverlapTracingEnabledRep();
}


//-------------------------------------------------------

// OnRep_Enable Cloth Overlap Tracing Rep

void UVertexPaintClothOverlapComponent::OnRep_ClothOverlapTracingEnabledRep() {

	if (clothOverlapTracingEnabled)
		ActivateClothOverlapTracing();
	else
		DeActivateClothOverlapTracing();
}


//-------------------------------------------------------

// Activate Cloth Overlap Tracing

void UVertexPaintClothOverlapComponent::ActivateClothOverlapTracing() {

	if (!parentSkeletalMeshComponent && clothOverlapTracingEnabled) return;


	const FSkeletalMeshLODRenderData& skelMeshRenderDataTemp = parentSkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[0];

	if (skelMeshRenderDataTemp.HasClothData()) {

		USkeletalMesh* skelMeshTemp = nullptr;

#if ENGINE_MAJOR_VERSION == 4

		skelMeshTemp = parentSkeletalMeshComponent->SkeletalMesh;

#elif ENGINE_MAJOR_VERSION == 5

	#if ENGINE_MINOR_VERSION == 0

		skelMeshTemp = parentSkeletalMeshComponent->SkeletalMesh.Get();

	#else

		skelMeshTemp = parentSkeletalMeshComponent->GetSkeletalMeshAsset();

	#endif
#endif

		parentSkeletalMeshComponentClothingAssets = skelMeshTemp->GetMeshClothingAssets();

		clothBoneTransformsInComponentSpaceTemp.Empty();
		clothBoneQuaternionsInComponentSpaceTemp.Empty();

		for (auto clothAsset : parentSkeletalMeshComponentClothingAssets) {

			if (auto clothingAssetCommonTemp = Cast<UClothingAssetCommon>(clothAsset)) {

				if (clothingAssetCommonTemp->UsedBoneNames.IsValidIndex(0)) {

					// Get Bone Transform in World Space
					auto boneTransformTemp = parentSkeletalMeshComponent->GetBoneTransform(parentSkeletalMeshComponent->GetBoneIndex(clothingAssetCommonTemp->UsedBoneNames[0]));

					// Transforms from World to Component Local
					boneTransformTemp.SetLocation(UKismetMathLibrary::InverseTransformLocation(parentSkeletalMeshComponent->GetComponentTransform(), boneTransformTemp.GetLocation()));
					boneTransformTemp.SetRotation((FQuat)UKismetMathLibrary::InverseTransformRotation(parentSkeletalMeshComponent->GetComponentTransform(), boneTransformTemp.GetRotation().Rotator()));

					clothBoneTransformsInComponentSpaceTemp.Add(clothAsset, boneTransformTemp);

					clothBoneQuaternionsInComponentSpaceTemp.Add(clothAsset, parentSkeletalMeshComponent->GetBoneQuaternion(clothingAssetCommonTemp->UsedBoneNames[0], EBoneSpaces::ComponentSpace));
				}
			}
		}


		SetComponentTickEnabled(true);
	}
}


//-------------------------------------------------------

// DeActivate Cloth Overlap Tracing

void UVertexPaintClothOverlapComponent::DeActivateClothOverlapTracing() {

	if (!parentSkeletalMeshComponent) return;


	for (auto& cachedOverlappCompTMapTemp : clothOverlappingActorsAndComponent) {

		for (auto cachedOverlapCompArrayTemp : cachedOverlappCompTMapTemp.Value.overlappingPrimitiveComponents) {

			if (UKismetSystemLibrary::DoesImplementInterface(cachedOverlapCompArrayTemp->GetOwner(), UVertexPaintDetectionInterface::StaticClass()))
				IVertexPaintDetectionInterface::Execute_ClothEndOverlappingMesh(cachedOverlapCompArrayTemp->GetOwner(), cachedOverlapCompArrayTemp, GetOwner(), parentSkeletalMeshComponent);
		}
	}

	clothOverlappingActorsAndComponent.Empty();

	SetComponentTickEnabled(false);
}


//-------------------------------------------------------

// Update

void UVertexPaintClothOverlapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(parentSkeletalMeshComponent) || clothObjectsToSphereTrace.Num() <= 0 || !clothOverlapTracingEnabled) return;


	if (GetOwner()->HasAuthority()) {

		if (parentSkeletalMeshComponent) {

			const FSkeletalMeshLODRenderData& skelMeshRenderDataTemp = parentSkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[0];

			// If cloth simulation is de-activated then disables cloth overlapping
			if (!skelMeshRenderDataTemp.HasClothData()) {

				SetClothOverlapTracingEnabled(false);
			}

			// If skel mesh is set to not generate overlap events then we turn it off for this to so it's consistent
			else if (!parentSkeletalMeshComponent->GetGenerateOverlapEvents()) {

				SetClothOverlapTracingEnabled(false);
			}
		}

		// If Dedicated Server then doesn't do any actual traces since the cloth isn't actually moving for them
		if (UGameplayStatics::GetPlayerController(GetWorld(), 0)) {

			if (!UGameplayStatics::GetPlayerController(GetWorld(), 0)->IsLocalPlayerController())
				return;
		}
	}


	TMap<int32, FClothSimulData> clothingData_Local;

	if (IsInGameThread())
		clothingData_Local = parentSkeletalMeshComponent->GetCurrentClothingData_GameThread();
	else
		clothingData_Local = parentSkeletalMeshComponent->GetCurrentClothingData_AnyThread();


	TMap<AActor*, FVertexPaintClothOverlapComponentsStruct> clothCurrentOverlappingActorsAndComponents_Local;
	TArray<UPrimitiveComponent*> clothCurrentOverlappingComponentsTotal_Local;
	TArray<UPrimitiveComponent*> clothCurrentOverlappingComponentsFromTrace_Local;
	FHitResult clothHitResult_Local;
	TArray<AActor*> clothTraceActorsToIgnore_Local;
	clothTraceActorsToIgnore_Local.Add(GetOwner());

	for (auto& currentClothDataTemp : clothingData_Local) {

		for (int i = 0; i < currentClothDataTemp.Value.Positions.Num(); i += FMath::Clamp(clothVertexTraceIntervall, 1, clothVertexTraceIntervall)) {

			clothCurrentOverlappingComponentsFromTrace_Local.Empty();


#if ENGINE_MAJOR_VERSION == 4

			FVector clothVertexPos_Local = currentClothDataTemp.Value.Positions[i];


			// UE5 with Chaos Cloth required a conversion of the cloth positions and rotation to get them aligned to the bone the cloth is attached to
#elif ENGINE_MAJOR_VERSION == 5

			FVector clothVertexPos_Local = (FVector)currentClothDataTemp.Value.Positions[i];

			// Converts Cloth Component Position so it aligns Location and Rotation Wise correctly with bone it's attached to.
			if (clothBoneTransformsInComponentSpaceTemp.Contains(parentSkeletalMeshComponentClothingAssets[currentClothDataTemp.Key])) {

				// Rotates the Vector according to the bone it's attached to
				clothVertexPos_Local = UKismetMathLibrary::Quat_RotateVector(clothBoneQuaternionsInComponentSpaceTemp.FindRef(parentSkeletalMeshComponentClothingAssets[currentClothDataTemp.Key]), clothVertexPos_Local);

				// Since the Vertex Positions we get from Cloth is component 0, 0, 0 we had to add the bone component local here to get it at the right location, so we now have the correct rotation and location for each cloth vertex
				clothVertexPos_Local = clothVertexPos_Local + clothBoneTransformsInComponentSpaceTemp.FindRef(parentSkeletalMeshComponentClothingAssets[currentClothDataTemp.Key]).GetLocation();
			}
#endif

			clothVertexPos_Local = UKismetMathLibrary::TransformLocation(parentSkeletalMeshComponent->GetComponentTransform(), clothVertexPos_Local);


			EDrawDebugTrace::Type drawDebugTraceTypeTemp = EDrawDebugTrace::None;

			if (debugClothSphereTraces)
				drawDebugTraceTypeTemp = EDrawDebugTrace::ForOneFrame;


			if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), clothVertexPos_Local, clothVertexPos_Local, clothVertexTraceRadius, clothObjectsToSphereTrace, false, clothTraceActorsToIgnore_Local, drawDebugTraceTypeTemp, clothHitResult_Local, true, FLinearColor::Red, FLinearColor::Green, 0.05)) {

				if (clothHitResult_Local.Component.Get()) {

					// If the Mesh the Cloth is Overlapping is set to Overlap the Skeletal Mesh the Cloth is at. 
					if (clothHitResult_Local.Component.Get()->GetCollisionResponseToChannel(parentSkeletalMeshComponent->GetCollisionObjectType()) == ECollisionResponse::ECR_Overlap) {


#if ENGINE_MAJOR_VERSION == 4

						if (clothCurrentOverlappingActorsAndComponents_Local.Contains(clothHitResult_Local.Actor.Get()))
							clothCurrentOverlappingComponentsFromTrace_Local = clothCurrentOverlappingActorsAndComponents_Local.FindRef(clothHitResult_Local.Actor.Get()).overlappingPrimitiveComponents;

						clothCurrentOverlappingComponentsFromTrace_Local.Add(clothHitResult_Local.Component.Get());

						FVertexPaintClothOverlapComponentsStruct clothCurrentOverlappingComponentsFromTraceStruct_Local;
						clothCurrentOverlappingComponentsFromTraceStruct_Local.overlappingPrimitiveComponents = clothCurrentOverlappingComponentsFromTrace_Local;

						clothCurrentOverlappingActorsAndComponents_Local.Add(clothHitResult_Local.Actor.Get(), clothCurrentOverlappingComponentsFromTraceStruct_Local);

#elif ENGINE_MAJOR_VERSION == 5

						if (clothCurrentOverlappingActorsAndComponents_Local.Contains(clothHitResult_Local.GetActor()))
							clothCurrentOverlappingComponentsFromTrace_Local = clothCurrentOverlappingActorsAndComponents_Local.FindRef(clothHitResult_Local.GetActor()).overlappingPrimitiveComponents;

						clothCurrentOverlappingComponentsFromTrace_Local.Add(clothHitResult_Local.Component.Get());

						FVertexPaintClothOverlapComponentsStruct clothCurrentOverlappingComponentsFromTraceStruct_Local;
						clothCurrentOverlappingComponentsFromTraceStruct_Local.overlappingPrimitiveComponents = clothCurrentOverlappingComponentsFromTrace_Local;

						clothCurrentOverlappingActorsAndComponents_Local.Add(clothHitResult_Local.GetActor(), clothCurrentOverlappingComponentsFromTraceStruct_Local);
#endif

						// Adds to a total overlapped comp array here so we can more optimized do checks later
						clothCurrentOverlappingComponentsTotal_Local.Add(clothHitResult_Local.Component.Get());
					}
				}
			}
		}
	}

	// After looping through all Cloths and Sphere Traced we check if we should run End Overlap for meshes we're no longer overlapping and Begin Overlap for new Meshes

	TArray<UPrimitiveComponent*> totalOverlappedCachedComponents_Local;


	for (auto& cachedOverlappCompTMapTemp : clothOverlappingActorsAndComponent) {

		for (auto cachedOverlapCompArrayTemp : cachedOverlappCompTMapTemp.Value.overlappingPrimitiveComponents) {

			bool runEndOverlap_Local = false;

			// Adds to a total cached components array which we can use in the next for loop to make it more optimized to check if any new components is overlapping
			totalOverlappedCachedComponents_Local.Add(cachedOverlapCompArrayTemp);

			// If current overlapped components doesn't contain the cached one, then runs End Overlap on the cached one
			if (!clothCurrentOverlappingComponentsTotal_Local.Contains(cachedOverlapCompArrayTemp))
				runEndOverlap_Local = true;


			if (runEndOverlap_Local) {

				if (UKismetSystemLibrary::DoesImplementInterface(cachedOverlapCompArrayTemp->GetOwner(), UVertexPaintDetectionInterface::StaticClass()))
					IVertexPaintDetectionInterface::Execute_ClothEndOverlappingMesh(cachedOverlapCompArrayTemp->GetOwner(), cachedOverlapCompArrayTemp, GetOwner(), parentSkeletalMeshComponent);
			}
		}
	}

	for (auto& currentOverlappCompTMapTemp : clothCurrentOverlappingActorsAndComponents_Local) {

		for (auto currentOverlapCompArrayTemp : currentOverlappCompTMapTemp.Value.overlappingPrimitiveComponents) {

			bool runBeginOverlap_Local = false;

			// If overlapped any components before
			if (totalOverlappedCachedComponents_Local.Num() > 0) {

				// If overlapped component wasn't part of the previous overlapped components, i.e. it's a new one then runs Begin Overlap on the new overlapped comp
				if (!totalOverlappedCachedComponents_Local.Contains(currentOverlapCompArrayTemp))
					runBeginOverlap_Local = true;
			}
			else
				runBeginOverlap_Local = true;


			if (runBeginOverlap_Local) {

				if (UKismetSystemLibrary::DoesImplementInterface(currentOverlapCompArrayTemp->GetOwner(), UVertexPaintDetectionInterface::StaticClass()))
					IVertexPaintDetectionInterface::Execute_ClothBeginOverlappingMesh(currentOverlapCompArrayTemp->GetOwner(), currentOverlapCompArrayTemp, GetOwner(), parentSkeletalMeshComponent);
			}
		}
	}

	clothOverlappingActorsAndComponent = clothCurrentOverlappingActorsAndComponents_Local;
}