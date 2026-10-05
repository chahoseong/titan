#include "Tests/TitanTestDamageableActor.h"

void ATitanTestDamageableActor::ReceiveDamage(float Damage, const FHitResult& Hit)
{
	ReceivedDamage += Damage;
}
