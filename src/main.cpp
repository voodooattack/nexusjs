/*
 * Nexus.js - The next-gen JavaScript platform
 * Copyright (C) 2016  Abdullah A. Hassan <abdullah@webtomizer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nexus.h"
#include "global_object.h"
#include <JavaScriptCore/runtime/InitializeThreading.h>
#include <JavaScriptCore/wasm/WasmFaultSignalHandler.h>

int main (int argc, const char ** argv)
{
  NX::Nexus nexus(argc, argv);
  struct sigaction sa {};
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &sa, nullptr);
  sigaction(SIGPIPE, &sa, nullptr);
  if (!setlocale(LC_ALL, "")) {
    WTFLogAlways("Locale not supported by C library.\n\tUsing the fallback 'C' locale.");
  }
  if (Gigacage::canPrimitiveGigacageBeDisabled())
    Gigacage::disablePrimitiveGigacage();
  WTF::initializeMainThread();
  WTF::initializeThreading();
  JSC::initializeThreading();
#if ENABLE_WEBASSEMBLY
  JSC::Wasm::enableFastMemory();
#endif
  return nexus.run();
}
