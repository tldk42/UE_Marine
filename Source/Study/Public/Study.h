// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMarine, Log, All)

#define MRLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) +TEXT(")"))
#define MRLOG_S(Verbosity) UE_LOG(LogMarine, Verbosity, TEXT("%s"), *MRLOG_CALLINFO)
#define MRLOG(Verbosity, Format, ...) UE_LOG(LogMarine, Verbosity, TEXT("%s%s"), *MRLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define MRCHECK(Expr, ...) { if(!Expr)) {MRLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}