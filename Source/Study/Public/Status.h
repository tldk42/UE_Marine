#pragma once

UENUM()
enum Status
{
	Stopped UMETA(DisplayName="Stopped"),
	Attacking UMETA(DisplayName="Attacking"),
	Moving UMETA(DisplayName="Moving"),
};