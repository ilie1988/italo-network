#ifndef LLARP_UTIL_ITALONET_INIT_H
#define LLARP_UTIL_ITALONET_INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef Italonet_INIT
#if defined(_WIN32)
#define Italonet_INIT \
  DieInCaseSomehowThisGetsRunInWineButLikeWTFThatShouldNotHappenButJustInCaseHandleItWithAPopupOrSomeShit
#else
#define Italonet_INIT _italonet_non_shit_platform_INIT
#endif
#endif

  int
  Italonet_INIT(void);

#ifdef __cplusplus
}
#endif
#endif