#include "dobby_internal.h"
#include "Interceptor.h"

__attribute__((constructor)) static void ctor() {
  DLOG(-1, "================================");
  DLOG(-1, "Dobby");
  DLOG(-1, "================================");

  DLOG(-1, "dobby in debug log mode, disable with cmake flag \"-DDOBBY_DEBUG=OFF\"");
}

PUBLIC const char *DobbyBuildVersion() {
  return "";
}

PUBLIC int DobbyDestroy(void *address) {
  // check if we already hook
  HookEntry *entry = Interceptor::SharedInstance()->findHookEntry((addr_t)address);
  if (entry) {
    uint8_t *buffer = entry->origin_insns;
    uint32_t buffer_size = entry->origin_insn_size;
#if defined(TARGET_ARCH_ARM)
    address = (void *)((addr_t)address - 1);
#endif
    CodePatch(address, buffer, buffer_size);
    Interceptor::SharedInstance()->removeHookEntry((addr_t)address);
    return RT_SUCCESS;
  }

  return RT_FAILED;
}
