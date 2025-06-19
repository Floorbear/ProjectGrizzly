# ProjectGrizzly

> UE5 기반 1인칭 협동 FPS | 오픈월드 익스트렉션 슈터

---

##  프로젝트 개요

Project Grizzly는 Unreal Engine 5를 기반으로 개발된 오픈월드 협동 FPS 게임입니다. 플레이어는 AI 혹은 동료 유저와 함께 루팅, 전투, 탈출을 수행하며 몰입감 있는 생존 경험을 제공합니다.

이 프로젝트의 핵심 목표는 **AI와 상호작용하는 몰입형 전투**, **루팅 및 탈출 기반의 전략적 게임플레이**, 그리고 **GAS 기반의 캐릭터 모듈화 구조**입니다.

---

##  시연 영상

- https://www.youtube.com/watch?v=Jp2hhS6l_QY
  
---
##  주요 특징

### 1. **GAS(Gameplay Ability System) 기반 캐릭터 설계**
- 사격, 정조준, 재장전, 질주, 기울이기 등 모든 행동을 어빌리티화
- 각 행동은 Net Execution Policy를 통해 클라이언트/서버 간 적절히 분산 실행

### 2. **전술 사격 시스템**
- 무기별 반동 곡선을 통한 현실감 있는 총기 제어
- 부위별 피격 처리 및 피드백 반영 (사운드/이펙트/리액션)

### 3. **자율 행동 기반 AI 시스템**
- Behavior Tree + EQS로 구성
- 위협도 기반 행동 전환 (순찰, 공격, 엄폐)
- 타겟 부위 정밀 감지 및 사격 구현 (예: 팔, 다리 사격)

### 4. **멀티플레이 및 네트워크**
- Dedicated Server 기반 서버-클라이언트 구조
- RPC 및 Replication을 통한 인벤토리/어빌리티 동기화
- 상자 소유권 처리 및 우회 RPC 적용

### 5. **인벤토리 시스템**
- 주무기/보조무기/소모품 슬롯 기반 구조
- 아이템 단일 인스턴스 및 수량 기반 분리
- 아이템 간 상호작용, 툴팁, UI 포함

### 6. **오픈월드 시스템**
- LandMass 기반 Landscape 구성
- 팩션 및 진영별 거점 시스템
- IInteractable 인터페이스 기반 상호작용 설계

---

## 기술 스택

| 항목           | 상세 내용                                                                 |
|----------------|--------------------------------------------------------------------------|
| 언리얼 버전     | Unreal Engine 5.0.3                                                        |
| 네트워크        | Dedicated Server, RPC, Replication                                       |
| 캐릭터 시스템    | Gameplay Ability System (GAS), AttributeSet                             |
| AI             | Behavior Tree, EQS, AIPerceptionComponent, Blackboard                    |
| UI             | UMG, TileView 기반 인벤토리 UI                                          |
| 기타 기술       | VectorCurve 기반 반동 구현, IK 리타깃팅, AnimNotify 기반 이벤트 트리거     |

---

## 설치 및 실행

1. 서버 다운로드 : https://drive.google.com/file/d/165U8YYTWuHxuPJlpSis6jMkz8tSav-8g/view?usp=sharing
2. 클라이언트 다운로드 : https://drive.google.com/file/d/1RirW2wNALXqVSdz5vJx95lZJ1X_x__6o/view?usp=sharing
3. 서버 구동 : WindowsServer - StartServer.bat 실행
4. 클라이언트 실행 : WindowsClient - ProjectGrizzlyClient.exe 실행
5. 서버 접속 : Go To Zone - 서버IP 입력 후 서버 접속 (기본 포트 7777)

---

##  개발자 Email

floorbear2723@gmail.com



