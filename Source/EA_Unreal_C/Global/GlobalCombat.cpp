#include "GlobalCombat.h"

void FCharacterStat::ResetCharacterStat()
{
	CurHP = MaxHP;
	CurStamina = MaxStamina;

}
float FCharacterStat::TakeDamage(float Damage)
{
	return CurHP -= Damage;
}