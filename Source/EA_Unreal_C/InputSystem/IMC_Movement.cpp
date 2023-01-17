// Fill out your copyright notice in the Description page of Project Settings.


#include "IMC_Movement.h"

#include "IA_Movement.h"
#include "IA_Look.h"
#include "IA_Sprint.h"
#include "IA_Jump.h"

UIMC_Movement::UIMC_Movement()
{
	/* Movement */
	{
		static UIA_Movement* movement = NewObject<UIA_Movement>();
		if(!MovementAction) MovementAction = movement;
		/* W */
		{
			this->MapKey(MovementAction, FKey("W"));
			this->GetMapping(0).Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
		}
		{
			this->MapKey(MovementAction, FKey("S"));
			this->GetMapping(1).Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
			this->GetMapping(1).Modifiers.Add(NewObject<UInputModifierNegate>());
		}
		{
			this->MapKey(MovementAction, FKey("A"));
			this->GetMapping(2).Modifiers.Add(NewObject<UInputModifierNegate>());
		}
		{
			this->MapKey(MovementAction, FKey("D"));
		}
	}
	/* Look */
	{
		static UIA_Look* look = NewObject<UIA_Look>();
		if (!LookAction)
		{
			LookAction = look;
			this->MapKey(LookAction, FKey("Mouse2D"));
			UInputModifierNegate* Negate = NewObject<UInputModifierNegate>();
			Negate->bX = false;
			Negate->bY = true;
			Negate->bZ = false;
			this->GetMapping(4).Modifiers.Add(Negate);
		}
	}
	/* Sprint */
	{
		static UIA_Sprint* sprint = NewObject<UIA_Sprint>();
		if (!SprintAction)
		{
			SprintAction = sprint;
			this->MapKey(SprintAction, FKey("LeftShift"));
		}
	}
	/* Jump */
	{
		static UIA_Jump* jump = NewObject<UIA_Jump>();
		if (!JumpAction)
		{
			JumpAction = jump;
			this->MapKey(JumpAction, FKey("SpaceBar"));
		}
	}
}