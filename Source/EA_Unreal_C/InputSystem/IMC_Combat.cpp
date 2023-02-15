#include "IMC_Combat.h"

#include "IA_Equip.h"
#include "IA_LMouse.h"
#include "IA_RMouse.h"
#include "IA_Catch.h"
#include "IA_Guard.h"
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
	/* LMouse */
	{
		static UIA_LMouse* action = NewObject<UIA_LMouse>();
		if (!LMouseAction)
		{
			LMouseAction = action;
			this->MapKey(LMouseAction, FKey("LeftMouseButton"));
		}
	}
	/* RMouse */
	{
		static UIA_RMouse* action = NewObject<UIA_RMouse>();
		if (!RMouseAction)
		{
			RMouseAction = action;
			this->MapKey(RMouseAction, FKey("RightMouseButton"));
		}
	}
	/* Catch */
	{
		static UIA_Catch* action = NewObject<UIA_Catch>();
		if (!CatchAction)
		{
			CatchAction = action;
			this->MapKey(CatchAction, FKey("Q"));
		}
	}
	/* Guard */
	{
		static UIA_Guard* guard = NewObject<UIA_Guard>();
		if (!GuardAction)
		{
			GuardAction = guard;
			this->MapKey(GuardAction, FKey("F"));
		}
	}
}
