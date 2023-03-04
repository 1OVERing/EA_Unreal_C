#include "GlobalCombat.h"
#include "Kismet/KismetMathLibrary.h"

void FCharacterStat::ResetCharacterStat()
{
	CurHP = MaxHP;
	CurStamina = MaxStamina;
}
float FCharacterStat::TakeDamage(float Damage)
{
	return CurHP -= Damage;
}

float FCharacterStat::TakeStaminaPoint(float Damage)
{
	return CurStamina = UKismetMathLibrary::FClamp(CurStamina - Damage, 0.f, MaxStamina);
}