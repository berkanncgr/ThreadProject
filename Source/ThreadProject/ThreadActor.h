// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "ThreadActor.generated.h"

UCLASS()
class THREADPROJECT_API AThreadActor : public AActor
{
	GENERATED_BODY()

public:

	AThreadActor();
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RunPrimeTask(int32 PrimeCount);

	UFUNCTION(BlueprintCallable)
	void RunPrimeTaskOnMain(int32 PrimeCount);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsThreadTask;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	class UTimelineComponent* Timeline;

	FOnTimelineFloat InterpFunction{};
	FOnTimelineEvent TimelineFinished{};

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Box;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UPROPERTY(EditAnywhere,Category ="Timeline")
	class UCurveFloat* Curve;

	UPROPERTY(EditAnywhere,Category ="Timeline")
	FVector StartLocation;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	FVector EndLocation;

	UPROPERTY(EditInstanceOnly, meta = (AlowPrivateAccess = "true"))
	int32 SearchCount;

	float ZOffset;

private:
	
	UFUNCTION()
	void TimelinePlay(float Value);
	
	UFUNCTION()
	void TimelineFinish();

	UFUNCTION()
	void Init();

};

// ======================================================================================

class PrimeSearchTask : public FNonAbandonableTask
{
public:
	PrimeSearchTask(int32 _primesearch);
	
	~PrimeSearchTask();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(PrimeSearchTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork();

	void DoWorkMain();

	int PrimeCount;
};