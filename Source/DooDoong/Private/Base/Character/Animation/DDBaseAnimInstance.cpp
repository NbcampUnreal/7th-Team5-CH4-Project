#include "Base/Character/Animation/DDBaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDDBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	CachedCharacter = Cast<ACharacter>(GetOwningActor());
	if (CachedCharacter)
	{
		CachedMovementComponent = Cast<UCharacterMovementComponent>(CachedCharacter->GetCharacterMovement());
	}
}

void UDDBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CachedCharacter || !CachedMovementComponent) return; 
	
	Velocity = CachedCharacter->GetVelocity();
	GroundSpeed = Velocity.Size2D();
	bShouldMove = GroundSpeed > 0.1f && !CachedMovementComponent->GetCurrentAcceleration().IsNearlyZero(); 
	bIsFalling = CachedMovementComponent->IsFalling(); 
}
