# DooDoong (두둥)

> 언리얼 엔진 5 기반 멀티플레이어 파티 보드게임  
> 보드 이동 + 미니게임 조합, 트로피를 획득하면 우승

### 🎥 프로젝트 시연 영상
[![내일배움캠프 언리얼 7기 - 멀티플레이 게임 프로젝트 5조 [DooDoong] 시연 영상](https://img.youtube.com/vi/kGtGvdYL8T0/0.jpg)](https://youtu.be/kGtGvdYL8T0?si=LuZ4BEtYZroyZAi1)
---

## 프로젝트 정보

| 항목   | 내용                          |
|------|-----------------------------|
| 엔진   | Unreal Engine 5.6           |
| 언어   | C++                         |
| 장르   | 파티 보드게임 (멀티플레이)             |
| 인원   | 최대 4인                       |
| 네트워크 | 전용 서버 (Dedicated Server)    |
| 팀    | 턱끈펭귄들                       |
| 개발기간 | 26.03.20 ~ 26.04.24 (약 1개월) |

---

## 팀 역할 구성

| 이름 | 역할 | 담당 작업 | GitHub |
|---|---|---|---|
| 박민재 | 팀장 | 인벤토리, 미니게임(플랫포머) | Daype1015 |
| 권민성 | QA | GAS 기반 플레이어 시스템, UI, 미니게임(컴뱃) | ZodiacRay |
| 정주현 | Git Master | 미니게임 코어 시스템, 아이템, 사운드, 미니게임(슈터) | wjdwngus2056-jpg |
| 최윤서 | PM | 보드게임 주사위 및 타일, 아이템, 미니게임(캐치, 배터) | yoonseo4343 |
| 최준혁 | 서기 | 보드게임 코어 시스템 | OptimalTime99 |
| 김상훈 | 레벨 디자인 | 보드게임 레벨 디자인 | kimsanghoon121 |

---

## 게임 흐름

```
로비 입장
  └─ 보드게임 라운드
       ├─ 아이템 사용
       ├─ 주사위 굴리기
       └─ 타일 효과 발동
  └─ 미니게임 (라운드 간)
       └─ 순위에 따라 코인 / 트로피 보정
  └─ 반복 → 트로피 목표 달성 시 우승
```

---

## 보드게임

### 타일 종류

| 타일 | 효과 |
|------|------|
| Normal | 효과 없음 |
| Goal | 트로피 획득 |
| Coin | 코인 획득 |
| Item | 랜덤 아이템 획득 |
| Damage | 체력 감소 |
| Move | 추가 이동 |

### 아이템

| 아이템 | 유형        | 효과 |
|--------|-----------|------|
| Healing Kit | Instant   | 자신 체력 회복 |
| Bomb | Targeting | 범위 내 플레이어 피해 |
| Portal | Targeting | 대상과 위치 교환 |
| Magnet | Range     | 대상 코인 탈취 |
| Melee Damage | Range     | 근접 범위 피해 |

---

## 미니게임

| 이름 | 장르 | 내용                 |
|------|------|--------------------|
| Platformer | Race | 장애물 피해 목표 지점 먼저 도달 |
| Shooter | Competition | 타겟 사격, 점수 경쟁       |
| Catch | Competition | 다이아몬드 수집  |
| Batter | Competition | 연타 점수 경쟁           |
| Combat | Competition | 플레이어 간 직접 전투       |

---

## 주요 시스템

### Gameplay Ability System (GAS)

- `UDDAbilitySystemComponent` — 커스텀 ASC
- `DDHealthSet` / `DDMovementSet` / `DDPointSet` — 체력, 이동속도, 트로피/코인
- 어빌리티: 주사위 굴리기, 타일 효과, 아이템 발동
- Gameplay Tags 100+ 개로 상태 관리 (Attacking, Death, Stun 등)

### 네트워크

- 전용 서버 권위 모델
- GAS 어트리뷰트 & 인벤토리 Replication
- NetMulticast RPC로 애니메이션 동기화
- Seamless Travel로 보드 ↔ 미니게임 맵 전환

### 인벤토리

- `UDDInventoryComponent` — 서버 복제 아이템 관리
- DataTable 기반 아이템 데이터 (`FItemTableRow`)
- 그리드 UI (`DDInventoryWidget`)

### 미니게임 관리

- `UDDMiniGameManager` (GameInstanceSubsystem) — 미니게임 생명주기 관리
- `UDDMiniGameDefinition` (Primary Data Asset) — 맵, 게임모드, 규칙 정의
- 장르 태그: Competition / Survival / Race

---

## 기술 스택

- Unreal Engine 5.6
- Gameplay Ability System (GAS)
- Enhanced Input System
- UMG
- DataTable 기반 데이터 드리븐 설계

---

## 프로젝트 구조

```
Source/DooDoong/
├── AbilitySystem/    ASC, Ability, AttributeSet
├── Base/             Character, GameMode, PlayerController 베이스
├── BoardGame/        보드판, 타일, 캐릭터, 어빌리티
├── MiniGames/        Platformer, Shooter, Catch, Batter, Combat
├── System/           GameInstance, MiniGameManager, UIManager
├── Data/             DataTypes, Config, AbilitySet
├── Input/            입력 설정
├── Lobby/            로비 모드
└── UI/               HUD, 인벤토리 위젯
```
