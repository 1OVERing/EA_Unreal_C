#include "IMC_Combat.h"

#include "IA_Equip.h"

UIMC_Combat::UIMC_Combat()
{

	/* Equip */
	{
		static UIA_Equip* equip = NewObject<UIA_Equip>();
		if (!EquipAction)
		{
			EquipAction = equip;
			this->MapKey(EquipAction, FKey("E"));
		}
	}
}
