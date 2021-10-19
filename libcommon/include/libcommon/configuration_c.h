#pragma once
#ifndef __CONFIGURATION_C_H__
#define __CONFIGURATION_C_H__
#include <libcommon/server_settings_c.h>

#ifdef NOEXCEPT
#define PREVIOUS_DEFINE_NOEXCEPT NOEXCEPT
#undef NOEXCEPT
#endif

#ifdef __cplusplus
extern "C" {
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

const struct server_settings * loadServerSettings(const char* configPath) NOEXCEPT;
const struct server_settings * getServerSettings() NOEXCEPT;
void saveDefaultConfig(const char* configPath) NOEXCEPT;

#ifdef __cplusplus
}
#endif

#ifdef PREVIOUS_DEFINE_NOEXCEPT
#define NOEXCEPT PREVIOUS_DEFINE_NOEXCEPT 
#undef PREVIOUS_DEFINE_NOEXCEPT
#endif

#endif // __CONFIGURATION_C_H__