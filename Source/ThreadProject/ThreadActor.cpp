// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CharacterBase.h"


// Sets default values
AThreadActor::AThreadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box -> SetupAttachment(Mesh);

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	InterpFunction.BindUFunction(this,FName("TimelinePlay"));
	TimelineFinished.BindUFunction(this,FName("TimelineFinish"));

	ZOffset=50.f;
}

void AThreadActor::BeginPlay()
{
	Super::BeginPlay();

	Init();
	Box->OnComponentBeginOverlap.AddDynamic(this,&AThreadActor::OnBoxBeginOverlap);
}



void AThreadActor::Init()
{
	if(!Curve) return;

	Timeline->AddInterpFloat(Curve, InterpFunction, FName("Alpha"));
	Timeline->SetTimelineFinishedFunc(TimelineFinished);

	StartLocation = GetActorLocation();
	EndLocation = FVector(StartLocation.X,StartLocation.Y,StartLocation.Z + ZOffset);

	Timeline->SetLooping(false);
	Timeline->SetIgnoreTimeDilation(true);

	Timeline->Play();
}


void AThreadActor::TimelinePlay(float Value)
{
	SetActorLocation(FMath::Lerp(StartLocation,EndLocation,Value));
}


void AThreadActor::TimelineFinish()
{
	if(Timeline->GetPlaybackPosition()==0.f)
	{
		Timeline->Play();
	}

	else if (Timeline->GetPlaybackPosition() == Timeline->GetTimelineLength())
	{
		Timeline->Reverse();
	}

}

void AThreadActor::OnBoxBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if(bIsThreadTask)
	{
		UE_LOG(LogTemp, Error, TEXT("Thread"));
		RunPrimeTask(SearchCount);
	}
	else if (!bIsThreadTask)
	{
		UE_LOG(LogTemp, Error, TEXT("Main"));
		RunPrimeTaskOnMain(SearchCount);
	}
}

	void AThreadActor::RunPrimeTask(int32 PrimeCount)
{
	(new FAutoDeleteAsyncTask<PrimeSearchTask>(PrimeCount))->StartBackgroundTask();
}

void AThreadActor::RunPrimeTaskOnMain(int32 PrimeCount)
{
	PrimeSearchTask* task = new PrimeSearchTask(PrimeCount);
	task->DoWork();
	delete task;
}

	// --------------------------------------------------------------------------------------------------------------------

PrimeSearchTask::PrimeSearchTask(int32 _primesearch)
{
	PrimeCount = _primesearch;
}

PrimeSearchTask::~PrimeSearchTask()
{
	UE_LOG(LogTemp,Warning,TEXT("Destructor called."));
}

void PrimeSearchTask::DoWork()
{


	int PrimesFound=0, CurrentTestNumber=2;

	while(PrimesFound < PrimeCount)
	{	
		bool bIsPrime=true;
		for (int32 i=2;i< CurrentTestNumber/2;i++)
		{
			if(CurrentTestNumber % i ==0)
			{
				bIsPrime=false;
				break;
			}

		}
		
		if(bIsPrime)
		{
			PrimesFound++;
			
			if(PrimesFound % 1000 == 0)
			{
				GEngine->AddOnScreenDebugMessage(0, 3, FColor::Red, FString::Printf(TEXT("Prime found :%i"), PrimesFound));
				UE_LOG(LogTemp,Warning,TEXT("Prime found %i."),PrimesFound);
			}
		}
		CurrentTestNumber++;
	}
}

void PrimeSearchTask::DoWorkMain()
{
	DoWork();
}