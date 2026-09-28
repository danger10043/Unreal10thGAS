// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetTest01.generated.h"

class USphereComponent;

UCLASS()
class UNREAL10THGAS_API ANetTest01 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetTest01();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, Category = "Test|Connection")
	void ApplyTargetToOwner();

protected:
	UPROPERTY(EditInstanceOnly, Category = "Test|Connection")
	TObjectPtr<ACharacter> Target;

	UPROPERTY(VisibleAnywhere, Category = "Test|Owner")
	TObjectPtr<USphereComponent> OverlapCollision;

};

/*
멀티플레이?
- 서버와 서버로 연결된 클라이언트 사이의 네트워크를 통해
- 원거리에 있는 플레이어가 함께 플레이하는 것

서버
- 모든 플레이어가 속한 가상의 게임월드를 유지하고,
- 게임 규칙을 집행하며,
- 참여자들간의 데이터를 중재하는 중앙 시스템(프로그램, 컴퓨터)
- 주요 역할
	- 권위(Authority) : 각종 핵심 로직의 계산과 판정 처리
	- 동기화(Replication) : 클라이언트가 제공하는 정보를 바탕으로 게임월드의 상태 갱신 및 전송 역할.
	- 세션 관리 : 플레이어들의 연결 관리

클라이언트
- 각 플레이어의 로컬 기기(PC, 콘솔, 모바일 등등)에서 실행되고,
- 사용자 입력을 받아 서버에 전달하고 결과를 화면과 소리 등으로 표현하는 프로그램.
- 주요 역할
	- 입력 수집 : 각종 입력을 서버로 전송
	- 랜더링 및 각종 피드백 : 화면 출력 및 반응 처리
	- 클라이언트 예측 및 보정 : 네트워크 지연 보정

서버의 종류
- 데디케이트 서버
	- 독립된 별도의 서버 존재.
	- 서버측 로직만 처리하는 프로그램(랜더링 안함)
- 리슨 서버
	- 클라이언트이자 서버.
	- 클라이언트 중 하나가 서버 역할도 함께 처리.

언리얼 주요 클래스
- AGameMode : 서버에만 존재(유일)
- AGameState : 서버와 클라이언트 모두에 존재
- APlayerState : 서버와 클라이언트 모두가 모든 플레이어의 것을 가지고 있어야 한다.
- APlayerController
	- 서버는 모든 플레이어의 컨트롤러를 가짐,
	- 클라이언트는 자신의 컨트롤러만 가짐.
- APawn/ACharacter : 서버와 클라이언트 모두에 존재
- AHUD : 자신의 클라이언트만 가짐.


Connection
- 서버와의 연결
- 플레이어 컨트롤러는 기본적으로 서버와의 Connection을 가진다.
- 액터가 PlayerController를 Owner로 가지게 되면 오너의 커넥션을 빌려쓸 수 있다.( = RPC 호출 가능해짐)
- 서버-클라이언트 통신을 위해 필수

Role
- Local vs Remote
	- Local Role : 내 컴퓨터에서 이 액터는 무슨 역할인가?
	- Remote Role : 네트워크 건너편에 있는 상대방 컴퓨터에서 이 액터가 무슨 역할인가?
					(서버입장에서 Remote는 각 클라이언트, 클라이언트 입장에서 Remote는 서버)
- 역할의 종류(ENetRole)
	//enum ENetRole : int
	//{
	//		ROLE_None UMETA(DisplayName = "None"),
	//		ROLE_SimulatedProxy UMETA(DisplayName = "Simulated Proxy"),
	//		ROLE_AutonomousProxy UMETA(DisplayName = "Autonomous Proxy"),
	//		ROLE_Authority UMETA(DisplayName = "Authority"),
	//		ROLE_MAX UMETA(Hidden),
	//};
	- ROLE_Authority
		- 서버가 만들었다. 여기서 일어나는 것이 진실.
	- ROLE_AutonomousProxy
		- 서버가 만든 것은 아니지만 즉시 조종할 수 있다.(플레이어)
		- 클라이언트의 플레이어 컨트롤어와 오너쉽이 연결된 오브젝트
	- ROLE_SimulatedProxy
		- 서버가 보내주는대로 시뮬레이션 했다.
		- 주로 다른 사람의 네트워크 오브젝트
	- ROLE_None
		- 네트워크 오브젝트가 아니다.
*/