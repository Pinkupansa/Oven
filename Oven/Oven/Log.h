#pragma once
#include "Oven/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Oven{
    class OVEN_API Log{
        public:
            static void Init();
            inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() {  return s_CoreLogger; }
            inline static std::shared_ptr<spdlog::logger>& GetClientLogger() {  return s_ClientLogger; }

        private:
            static std::shared_ptr<spdlog::logger> s_CoreLogger;
            static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}


#define OVEN_CORE_TRACE(...) ::Oven::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OVEN_CORE_INFO(...) ::Oven::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OVEN_CORE_WARN(...) ::Oven::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OVEN_CORE_ERROR(...) ::Oven::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OVEN_CORE_FATAL(...) ::Oven::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#ifdef OVEN_ENABLE_ASSERT

#ifdef OVEN_PLATFORM_WINDOWS
#define OVEN_CORE_ASSERT(x, ...) { if(!(x)) { OVEN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define OVEN_ASSERT(x, ...) { if(!(x)) { OVEN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define OVEN_CORE_ASSERT(x, ...) { if(!(x)) { OVEN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __builtin_trap(); } }
#define OVEN_ASSERT(x, ...) { if(!(x)) { OVEN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __builtin_trap(); } }
#endif

#endif
#define OVEN_TRACE(...) ::Oven::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OVEN_INFO(...) ::Oven::Log::GetClientLogger()->info(__VA_ARGS__)
#define OVEN_WARN(...) ::Oven::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OVEN_ERROR(...) ::Oven::Log::GetClientLogger()->error(__VA_ARGS__)
#define OVEN_FATAL(...) ::Oven::Log::GetClientLogger()->fatal(__VA_ARGS__)
