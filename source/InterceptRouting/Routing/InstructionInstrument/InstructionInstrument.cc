#include "dobby_internal.h"

#include "Interceptor.h"
#include "InterceptRouting/InterceptRouting.h"
#include "InterceptRouting/Routing/InstructionInstrument/InstructionInstrumentRouting.h"

PUBLIC int DobbyInstrument(void *address, instrument_callback_t handler) {
  if (!address) {
    ERROR_LOG("address is 0x0.\n");
    return RS_FAILED;
  }

#if defined(__arm64__) && __has_feature(ptrauth_calls)
  address = ptrauth_strip(address, ptrauth_key_asia);
#endif

  RAW_LOG(1, "\n\n");
  DLOG(0, "----- [DobbyInstrument:%p] -----", address);

  // check if we already instruemnt
  auto entry = Interceptor::SharedInstance()->findHookEntry((addr_t)address);
  if (entry) {
    ERROR_LOG("%s already been instrumented.", address);
    return RS_FAILED;
  }

  entry = new HookEntry();
  entry->id = Interceptor::SharedInstance()->getHookEntryCount();
  entry->type = kInstructionInstrument;
  entry->patched_insn_addr = (addr_t)address;

  auto routing = new InstructionInstrumentRouting(entry, handler);
  routing->Prepare();
  routing->DispatchRouting();
  routing->Commit();

  Interceptor::SharedInstance()->addHookEntry(entry);

  return RS_SUCCESS;
}
