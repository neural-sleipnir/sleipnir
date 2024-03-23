#ifndef SLEIPNIR_SLEIPNIR_MACROS_H
#define SLEIPNIR_SLEIPNIR_MACROS_H

////////////////////////////////////////////////////////////////////////////////
// Cpp
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
#define SLEIPNIR_START_CPP_NAMESPACE \
  namespace sleipnir {               \
  extern "C" {
#define SLEIPNIR_END_CPP_NAMESPACE \
  }                                    \
  }
#else
#define SLEIPNIR_START_CPP_NAMESPACE
#define SLEIPNIR_END_CPP_NAMESPACE
#endif

////////////////////////////////////////////////////////////////////////////////
// DllImport
////////////////////////////////////////////////////////////////////////////////

#if defined(SLEIPNIR_OS_WINDOWS)
#ifdef SLEIPNIR_DLL
#define SLEIPNIR_DLL_EXPORT __declspec(dllexport)
#else
#define SLEIPNIR_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define SLEIPNIR_DLL_EXPORT
#endif // SLEIPNIR_OS_WINDOWS

#endif //SLEIPNIR_SLEIPNIR_MACROS_H
