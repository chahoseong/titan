# 데이터 시스템

언리얼 엔진에서 게임플레이 데이터를 담는 방법과 선택 기준을 정리한다.
플레이 중 바뀌지 않는 정의 데이터만 다룬다.

API와 지정자는 UE 5.8 엔진 소스의 헤더에서 확인했다. 예제의 타입 이름은 설명용이다.

## 1. 선택 기준

컨테이너, 참조, 식별은 서로 다른 선택이다. 각 표에서 따로 고른다.

### 1.1 컨테이너

| 조건 | 선택 |
|---|---|
| 항목을 에디터에서 하나씩 편집하고, 쓰는 쪽이 항목을 프로퍼티에 지정한다 | `UDataAsset` |
| 위와 같고, 코드에서 유형별 목록을 조회하거나, ID로 로드하거나, 번들 단위로 로드하거나, 명시적으로 언로드한다 | `UPrimaryDataAsset` |
| 항목이 다른 항목의 값을 물려받고 일부만 덮어쓴다 | Data Only Blueprint |
| 항목을 표에서 나란히 편집하거나 CSV, JSON으로 주고받는다 | Data Table |
| 입력값에 따라 달라지는 수치를 곡선으로 조정한다 | Curve Table 또는 `FRuntimeFloatCurve` |
| 한 프로퍼티에 종류가 다른 데이터 조각 중 하나를 골라 넣는다 | `TInstancedStruct` |
| 위와 같고, 조각을 블루프린트로 상속하거나, 조각에 `UFUNCTION`을 두거나, 다른 객체가 조각을 포인터로 참조한다 | Instanced Object |

### 1.2 참조

| 조건 | 선택 |
|---|---|
| 소유 데이터를 로드할 때 대상 에셋도 함께 로드한다 | 하드 참조 |
| 소유 데이터를 로드할 때 대상 에셋을 로드하지 않고, 필요한 시점에 따로 로드한다 | 소프트 참조 |

### 1.3 식별

| 조건 | 선택 |
|---|---|
| 값의 집합이 코드에 고정되어 있고, 한 번에 하나의 값만 가진다 | enum |
| 값을 계층으로 묶어 상위 값으로 비교하거나, 여러 값을 동시에 가지거나, 코드를 고치지 않고 값을 추가한다 | Gameplay Tags |

## 2. 컨테이너

### 2.1 Data Asset

C++ 클래스가 데이터의 구조를 정의하고, 에디터에서 만든 에셋이 값을 담는다.

```cpp
#include "Engine/DataAsset.h"

UCLASS(BlueprintType)
class UExampleDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Example", meta = (ClampMin = "0.0"))
	float Damage = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Example")
	TObjectPtr<UStaticMesh> Mesh;
};
```

| 클래스 | 접근 방법 | 블루프린트로 상속 |
|---|---|---|
| `UDataAsset` | 다른 객체의 프로퍼티에 에셋을 지정해서 참조한다 | 클래스에 `Blueprintable`을 붙여야 한다 |
| `UPrimaryDataAsset` | Asset Manager가 `FPrimaryAssetId`로 찾고 로드한다 | 할 수 있다 |

`UPrimaryDataAsset`을 쓸 때

- 프로젝트 설정의 Asset Manager에서 `PrimaryAssetTypesToScan`에 유형을 등록한다
- `UAssetManager::GetPrimaryAssetIdList`로 유형별 목록을 얻는다
- `UAssetManager::LoadPrimaryAssets`로 비동기 로드한다
- 소프트 참조 프로퍼티에 `meta = (AssetBundles = "번들이름")`을 붙이면 번들 단위로 함께 로드할 수 있다

제약

- 로드된 에셋은 모든 사용처가 같은 객체를 공유한다. 런타임에 값을 쓰지 않는다
- 에셋 인스턴스마다 함수를 재정의할 수 없다. 함수는 C++ 클래스에 둔다
- 에셋 인스턴스는 다른 인스턴스의 값을 물려받을 수 없다. 필요하면 Data Only Blueprint를 쓴다

### 2.2 Data Only Blueprint

C++ 클래스를 부모로 하는 블루프린트를 만들고 기본값만 설정한다. 자식 블루프린트는 부모의 값을 물려받고, 덮어쓴 값만 따로 저장한다.

- 부모 C++ 클래스에 `Blueprintable`을 붙인다. 이 지정자는 자식 클래스가 물려받는다
- 참조는 `TSubclassOf` 또는 `TSoftClassPtr`로 한다
- 값은 클래스 기본 객체(`GetDefaultObject`)에서 읽는다
- 블루프린트 그래프에 로직을 넣지 않는다

제약

- 클래스 기본 객체는 모든 사용처가 공유한다. 런타임에 값을 쓰지 않는다

### 2.3 Data Table

`FTableRowBase`를 상속한 구조체가 행의 구조를 정의한다.

```cpp
#include "Engine/DataTable.h"

USTRUCT(BlueprintType)
struct FExampleRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Example")
	float Damage = 0.0f;
};
```

```cpp
// Restrict the picker to tables whose row struct is FExampleRow
UPROPERTY(EditAnywhere, Category = "Example", meta = (RowType = "ExampleRow"))
FDataTableRowHandle Row;

UPROPERTY(EditAnywhere, Category = "Example", meta = (RequiredAssetDataTags = "RowStructure=/Script/Titan.ExampleRow"))
TObjectPtr<UDataTable> Table;
```

```cpp
const FExampleRow* Found = Row.GetRow<FExampleRow>(TEXT("ExampleContext"));
```

제약

- `GetRow`와 `FindRow`가 돌려주는 포인터는 테이블이 가진 행을 가리킨다. 런타임에 값을 쓰지 않는다
- 행 포인터를 함수의 지역 범위를 넘어 보관하지 않는다. 보관할 때는 `FDataTableRowHandle`을 쓰고, 값을 쓸 때 다시 조회한다
- 테이블 하나가 바이너리 에셋 하나다. 행 단위로 병합할 수 없다
- 테이블을 로드하면 모든 행의 하드 참조 대상이 함께 로드된다. 쓰지 않는 행의 대상도 포함한다
- 행에 Instanced Object를 넣지 않는다. 행에 종류가 다른 조각이 필요하면 `TInstancedStruct`를 쓴다

`UCompositeDataTable`은 행 구조가 같은 여러 테이블을 하나로 합친다. 사용법은 Data Table과 같다.

### 2.4 Curve Table

float 값을 곡선으로 담는다. CSV와 JSON을 지원한다.

```cpp
#include "Engine/CurveTable.h"

UPROPERTY(EditAnywhere, Category = "Example")
FCurveTableRowHandle DamageByDistance;
```

```cpp
const float Value = DamageByDistance.Eval(Distance, TEXT("ExampleContext"));
```

제약

- `GetCurve`와 `FindCurve`가 돌려주는 곡선 포인터를 함수의 지역 범위를 넘어 보관하지 않는다. 보관할 때는 `FCurveTableRowHandle`을 쓴다

곡선 하나만 필요하고 표로 관리할 필요가 없으면 프로퍼티에 `FRuntimeFloatCurve`(`Curves/CurveFloat.h`)를 직접 둔다.

### 2.5 Instanced Struct

기반 구조체를 상속한 구조체 중 하나를 에디터에서 골라 값을 채운다.

```cpp
#include "StructUtils/InstancedStruct.h"

USTRUCT(BlueprintType)
struct FExampleFragment
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FExampleFragment_Spread : public FExampleFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Example")
	float AngleDegrees = 0.0f;
};
```

```cpp
UPROPERTY(EditAnywhere, Category = "Example")
TInstancedStruct<FExampleFragment> Fragment;

UPROPERTY(EditAnywhere, Category = "Example")
TArray<TInstancedStruct<FExampleFragment>> Fragments;
```

```cpp
if (const FExampleFragment_Spread* Spread = Fragment.GetPtr<FExampleFragment_Spread>())
{
	// Use Spread->AngleDegrees
}
```

- `TInstancedStruct<T>`는 기반 구조체를 템플릿 인자로 지정한다
- `FInstancedStruct`는 기반 구조체를 `meta = (BaseStruct = "/Script/Titan.ExampleFragment")`로 지정한다. 경로의 구조체 이름에는 접두사 `F`를 붙이지 않는다
- `CoreUObject` 모듈에 들어 있다. 플러그인을 켜거나 의존 모듈을 추가하지 않는다
- 구조체도 가상 함수를 가질 수 있다
- 담긴 구조체는 실제 타입의 `UScriptStruct`로 생성되고 파괴된다

제약

- 구조체에 `UFUNCTION`을 둘 수 없다
- 블루프린트로 상속할 수 없다

### 2.6 Instanced Object

소유 객체 안에 UObject를 만들어 넣는다. 소유 객체와 함께 직렬화되고 함께 로드된다.

```cpp
UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories)
class UExampleBehavior : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Example")
	float Duration = 0.0f;
};
```

```cpp
UPROPERTY(EditAnywhere, Instanced, Category = "Example")
TObjectPtr<UExampleBehavior> Behavior;

UPROPERTY(EditAnywhere, Instanced, Category = "Example")
TArray<TObjectPtr<UExampleBehavior>> Behaviors;
```

- 조각을 블루프린트로 상속하려면 클래스에 `Blueprintable`을 붙인다

제약

- Instanced Struct보다 메모리를 더 쓴다
- Data Table의 행에 넣지 않는다
- Instanced Object를 담는 프로퍼티는 C++에서 `UPROPERTY(Instanced)`로 선언한다

## 3. 참조와 식별

### 3.1 하드 참조와 소프트 참조

| 종류 | 타입 | 대상이 로드되는 시점 |
|---|---|---|
| 하드 참조 | `TObjectPtr`, `TSubclassOf` | 소유 객체가 로드될 때 |
| 소프트 참조 | `TSoftObjectPtr`, `TSoftClassPtr` | 코드가 요청할 때 |

소프트 참조는 대상의 경로만 담는다.

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Example")
TSoftObjectPtr<UTexture2D> Icon;
```

| 방식 | 호출 |
|---|---|
| 동기 | `Icon.LoadSynchronous()` |
| 비동기 | `UAssetManager::GetStreamableManager().RequestAsyncLoad(Icon.ToSoftObjectPath(), Delegate)` |

동기 로드는 로드가 끝날 때까지 게임 스레드를 멈춘다.

소프트 참조의 비용

- 필요한 시점에 로드를 요청하는 코드가 필요하다
- 비동기 로드라면 완료를 처리하는 코드가 필요하다
- 로드가 끝나기 전에는 대상을 쓸 수 없다

### 3.2 Gameplay Tags

계층형 이름이다. 프로젝트에 등록된 태그만 쓸 수 있다.

준비

- [Titan.Build.cs](../../../Source/Titan/Titan.Build.cs)의 의존 모듈에 `GameplayTags`를 추가한다

C++에서 태그 정의

```cpp
// Header
#include "NativeGameplayTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Example_Damage_Fire);
```

```cpp
// Source
UE_DEFINE_GAMEPLAY_TAG(TAG_Example_Damage_Fire, "Example.Damage.Fire");
```

프로퍼티

```cpp
// Restrict the picker to tags under Example.Damage
UPROPERTY(EditAnywhere, Category = "Example", meta = (Categories = "Example.Damage"))
FGameplayTag DamageType;

UPROPERTY(EditAnywhere, Category = "Example")
FGameplayTagContainer Tags;
```

비교

| 함수 | `Example.Damage.Fire`를 `Example.Damage`와 비교한 결과 |
|---|---|
| `FGameplayTag::MatchesTag` | 참 |
| `FGameplayTag::MatchesTagExact` | 거짓 |
| `FGameplayTagContainer::HasTag` | 참 |
| `FGameplayTagContainer::HasTagExact` | 거짓 |

## 4. 편집과 검증

### 4.1 프로퍼티 지정자

| 지정자 | 효과 |
|---|---|
| `meta = (EditCondition = "bFlag")` | `bFlag`가 거짓이면 편집할 수 없다 |
| `meta = (EditCondition = "bFlag", EditConditionHides)` | `bFlag`가 거짓이면 숨긴다 |
| `meta = (InlineEditConditionToggle)` | bool을 별도 행 없이 조건 대상 프로퍼티의 체크박스로 표시한다 |
| `meta = (ClampMin = "0.0")` | 입력할 수 있는 최솟값을 정한다 |
| `meta = (Units = "cm")` | 단위를 표시한다 |
| `AssetRegistrySearchable` | 클래스가 직접 가진 프로퍼티의 값을 Asset Registry 태그로 내보낸다 |

### 4.2 데이터 검증

`UObject::IsDataValid`를 재정의한다. 에셋을 저장할 때 실행되고, 커맨드라인에서도 실행할 수 있다.

```cpp
#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult UExampleDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (Damage <= 0.0f)
	{
		Context.AddError(FText::FromString(TEXT("Damage must be greater than zero.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
```

### 4.3 Asset Registry 태그

Asset Registry는 에셋을 로드하지 않고 조회할 수 있는 정보를 담는다.

| 내보낼 값 | 방법 |
|---|---|
| 클래스가 직접 가진 프로퍼티의 값 | `AssetRegistrySearchable` |
| 구조체 안의 멤버, 계산한 값 | `GetAssetRegistryTags` 재정의 |

```cpp
#include "UObject/AssetRegistryTagsContext.h"

void UExampleDefinition::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
	Super::GetAssetRegistryTags(Context);

	Context.AddTag(FAssetRegistryTag(
		TEXT("Damage"),
		FString::SanitizeFloat(Damage),
		FAssetRegistryTag::TT_Numerical));
}
```

### 4.4 Property Matrix

여러 에셋을 표로 나열해 함께 편집한다. 콘텐츠 브라우저에서 에셋을 선택하고 우클릭 메뉴의 Asset Actions에서 연다.

## 5. 출처의 코드와 5.8의 차이

| 항목 | 출처 | 5.8 |
|---|---|---|
| Instanced Struct | Struct Utils 플러그인을 켠다 | `CoreUObject`에 있다. 플러그인은 5.5부터 폐기 표시가 붙었다 |
| Instanced Struct 선언 | `FInstancedStruct`와 `BaseStruct` 메타 | `TInstancedStruct<T>`를 쓸 수 있다 |
| `GetAssetRegistryTags` | `TArray<FAssetRegistryTag>&`를 받는다 | 이 버전은 5.4부터 폐기되었다. `FAssetRegistryTagsContext`를 받는다 |
| Instanced Object 프로퍼티 | 원시 포인터 | `TObjectPtr` |

## 6. 제외한 선택지

| 항목 | 이유 |
|---|---|
| `FScalableFloat` | GameplayAbilities 플러그인에 들어 있다. GAS는 쓰지 않는다. Curve Table이나 `FRuntimeFloatCurve`를 쓴다 |
| Data Registry | 여러 소스의 구조체 데이터를 ID로 조회하고, 비동기로 얻고, 캐시하는 기능이다. 여러 소스를 합치거나 데이터를 비동기로 얻는 요구가 범위에 없다. 플러그인은 베타 단계다 |
| HTTP 요청, Web API 플러그인 | 외부 데이터 소스가 범위에 없다 |
| Asset Metadata | 에셋을 로드해야 읽을 수 있다. 조회용 값은 Asset Registry 태그로 내보낸다 |

## 출처

- [Working with Data in Unreal Engine](https://dev.epicgames.com/community/learning/tutorials/Gp9j/working-with-data-in-unreal-engine-data-tables-data-assets-uproperty-specifiers-and-more) (Epic Developer Community)
