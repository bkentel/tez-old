#pragma once

//==============================================================================
// Compiler detection
//==============================================================================
#if defined(_MSC_FULL_VER)
#   define BK_COMPILER_MSVC
#endif

//==============================================================================
// Machine detection
//==============================================================================
#if defined(BK_COMPILER_MSVC)
#   if defined(_M_X64)
#       define BK_MACHINE_X64
#   elif defined(_M_IX86)
#       define BK_MACHINE_X86
#   endif
#endif

//==============================================================================
// Platform detection
//==============================================================================
#if defined(_WIN32)
#   define BK_PLATFORM_WINDOWS
#endif

//------------------------------------------------------------------------------
// Compiler specific defines for "deleted" functions.
//------------------------------------------------------------------------------
#if defined(BK_COMPILER_MSVC)
#   define BK_DELETE
#else
#   define BK_DELETE = delete
#endif

//------------------------------------------------------------------------------
// Compiler specific defines for nothrow.
//------------------------------------------------------------------------------
#if defined(BK_COMPILER_MSVC)
#   define BK_NOTHROW throw()
#   define BK_NOTHROW_IF(x)
#else
#   define BK_NOTHROW nothrow
#   define BK_NOTHROW_IF(x) nothrow((x))
#endif

//------------------------------------------------------------------------------
// Compiler specific defines unreachable code.
//------------------------------------------------------------------------------
#if defined(BK_COMPILER_MSVC)
#   if defined(NDEBUG)
#       define BK_UNREACHABLE __assume(0);
#   else
#       define BK_UNREACHABLE __debugbreak(); __assume(0);
#   endif
#else
#   error "BK_UNREACHABLE unimplemented"
#endif


#define BK_CONCAT_IMPL(a, b) a##b
#define BK_CONCAT(a, b) BK_CONCAT_IMPL(a, b)

#define BK_UNIQUE_ID_IMPL __COUNTER__

#define BK_UNIQUE_ID BK_CONCAT(bk_unique_id_, BK_UNIQUE_ID_IMPL)

#define BK_UNUSED(x) ((void)(x))
