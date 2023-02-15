#include "GlobalCombat.h"

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
	return CurGuardPoint -= Damage;
}
