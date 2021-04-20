#pragma once

#include <chrono>
#include <Windows.h>
#include <discord_rpc.h> 
#include <discord_register.h>

using namespace std;

class DCInstance {
public:
	static inline DiscordRichPresence Presence;

	static void InitializeRPC(const char*);

	static void UpdateDetails(const char*);
	static void UpdateState(const char*);
	static void UpdateLImage(const char*, const char*);
	static void UpdateSImage(const char*, const char*);
};