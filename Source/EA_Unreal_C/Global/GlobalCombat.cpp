#include "GlobalCombat.h"
#include "Kismet/KismetMathLibrary.h"

void FCharacterStat::ResetCharacterStat()
{
	CurHP = MaxHP;
	CurStamina = MaxStamina;
	CurGuardPoint = MaxGuardPoint;
}
float FCharacterStat::TakeDamage(float Damage)
{
	return CurHP -= Damage;
}

float FCharacterStat::TakeGuardPoint(float Damage)
{
	return CurGuardPoint = UKismetMathLibrary::FClamp(CurGuardPoint - Damage,0.f,MaxGuardPoint);
}
