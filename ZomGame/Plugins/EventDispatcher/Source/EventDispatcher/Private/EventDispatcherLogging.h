/**
* Copyright (C) 2017-2021 | eelDev (Dry Eel Development)
*/

#define LogDebug(format, ...) UE_LOG(LogEventDispatcher, Log, TEXT("[%s] " format), *FString(__FUNCTION__), ##__VA_ARGS__)
#define LogVerbose(format, ...) UE_LOG(LogEventDispatcher, Verbose, TEXT("[%s] " format), *FString(__FUNCTION__), ##__VA_ARGS__)
#define LogVeryVerbose(format, ...) UE_LOG(LogEventDispatcher, VeryVerbose, TEXT("[%s] " format), *FString(__FUNCTION__), ##__VA_ARGS__)
#define LogError(format, ...) UE_LOG(LogEventDispatcher, Error, TEXT("[%s] " format), *FString(__FUNCTION__), ##__VA_ARGS__)
#define LogWarning(format, ...) UE_LOG(LogEventDispatcher, Warning, TEXT("[%s] " format), *FString(__FUNCTION__), ##__VA_ARGS__)
