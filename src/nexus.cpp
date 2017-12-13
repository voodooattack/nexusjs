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
#include "scoped_context.h"
#include "scheduler.h"
#include "object.h"
#include "value.h"
#include "task.h"
#include "globals/global.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <cstring>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include <JavaScriptCore/JSContextRef.h>
#include <JavaScriptCore/API/APICast.h>
#include <JavaScriptCore/runtime/InitializeThreading.h>
#include <JavaScriptCore/parser/ParserError.h>
#include <JavaScriptCore/parser/SourceCode.h>
#include <JavaScriptCore/runtime/JSNativeStdFunction.h>
#include <JavaScriptCore/runtime/PromiseDeferredTimer.h>

namespace po = boost::program_options;

NX::Nexus::Nexus(int argc, const char ** argv):
  argc(argc), argv(argv), myArguments(), myContextGroup(nullptr), myMainContext(nullptr),
  myScriptSource(), myScriptPath(), myScheduler(nullptr), myOptions(), myClasses(), myExitStatus(0)
{
  for (int i = 0; i < argc; i++) {
    myArguments.emplace_back(std::string(argv[i]));
  }
}

NX::Nexus::~Nexus()
{
//  delete myMainContext;
  for(auto & c : myClasses)
    JSClassRelease(c.second);
  this->~noncopyable();
}

bool NX::Nexus::parseArguments()
{
  unsigned int nThreads = 0;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce this help message")
    ("version,v", "print version and exit")
    ("silent,s", "don't print errors")
    ("concurrency", po::value<unsigned int>(&nThreads)->default_value(boost::thread::hardware_concurrency()),
      "maximum threads in the task scheduler's pool (defaults to the available number of cores)")
    ("input-file", po::value<std::string>(), "input file");
  po::positional_options_description p;
  p.add("input-file", -1);
  po::variables_map & vm(myOptions);
  po::store(po::command_line_parser(this->argc, this->argv).options(desc).positional(p).run(), vm);
  po::notify(vm);
  if (this->argc <= 1 || vm.count("help")) {
    std::cout << "Nexus.js - The next-gen JavaScript platform" << std::endl;
    std::cout << std::endl;
    std::cout << desc << std::endl;
    return true;
  }
  myScriptPath = boost::filesystem::absolute(vm["input-file"].as<std::string>()).string();
  if (!boost::filesystem::exists(myScriptPath)) {
    std::cerr << "file not found: " << myScriptPath << std::endl;
    return true;
  }
  std::ifstream ifs(myScriptPath);
  myScriptSource.assign(std::istreambuf_iterator<char>(ifs),
                        std::istreambuf_iterator<char>());
  return false;
}

void NX::Nexus::ReportException(JSContextRef ctx, JSValueRef exception) {
  std::ostringstream stream;
  stream << std::endl;
  try {
    if (JSValueGetType(ctx, exception) != kJSTypeObject)
    {
      stream << "Unhandled exception: " << NX::Value(ctx, exception).toString() << std::endl;
    } else {
      NX::Object exp(ctx, exception);
      auto sourceURL = exp["sourceURL"], line = exp["line"], message = exp["message"], stack = exp["stack"];
      auto name = exp["name"];
      stream << "Unhandled " <<
                             (name->toBoolean() ? name->toString() : "Exception")
             << " in " << sourceURL->toString() << ":" << exp["line"]->toString() << std::endl;
      stream << "Message: " << exp["message"]->toString() << std::endl;
      stream << "Stack:\n" << exp["stack"]->toString() << std::endl;
    }
    stream << std::endl;
  } catch(const NX::Exception & e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
  }
  std::cerr << stream.str();
}

void NX::Nexus::ReportException(const std::exception &e) {
  std::ostringstream stream;
  stream << std::endl;
  stream << e.what() << std::endl;
  std::cerr << stream.str();
}

void NX::Nexus::initScheduler()
{
  auto concurrency = myOptions["concurrency"].as<unsigned int>();
  myScheduler.reset(new Scheduler(this, concurrency));
}

void NX::Nexus::run() {
  try {
    if (parseArguments()) {
      exit(0);
    }
    initScheduler();
    if (!myMainContext) {
      myMainContext = new NX::Context(nullptr, this);
    }
    JSC::Options::useWebAssembly() = true;
    JSC::Options::useWebAssemblyFastMemory() = true;
    JSC::Options::useFastTLSForWasmContext() = true;
// enable these to debug the module loader!
//    JSC::Options::dumpModuleLoadingState() = true;
//    JSC::Options::dumpModuleRecord() = true;
    auto promise = myMainContext->evaluateModule("import \"" + myScriptPath + "\";", nullptr, myScriptPath + "[entry]", 1);
    {
      JSC::JSLockHolder holder(myMainContext->vm());
      JSC::JSFunction *fulfillHandler = JSC::JSNativeStdFunction::create(*myMainContext->vm(),
                                                                         myMainContext->globalObject(),
                                                                         1, WTF::String(),
                                                                         [&](JSC::ExecState *exec)
      {
        myExitStatus = 0;
        return JSC::JSValue::encode(JSC::jsUndefined());
      });
      JSC::JSFunction *rejectHandler = JSC::JSNativeStdFunction::create(*myMainContext->vm(),
                                                                        myMainContext->globalObject(),
                                                                        1, String(),
                                                                        [&](JSC::ExecState *exec)
      {
        ReportException(toRef(exec), toRef(exec, exec->argument(0)));
        myExitStatus = 1;
        return JSC::JSValue::encode(JSC::jsUndefined());
      });
      promise->then(myMainContext->globalObject()->globalExec(), fulfillHandler, rejectHandler);
    }
    myScheduler->start();
    myScheduler->joinPool([&] {
      myMainContext->vm()->promiseDeferredTimer->runRunLoop();
      {
        JSC::JSLockHolder holder(myMainContext->vm());
        myMainContext->vm()->drainMicrotasks();
      }
    });
    myScheduler->join();
    exit(myExitStatus);
  } catch(std::exception & e) {
    ReportException(e);
    exit(1);
  }
}

void NX::Nexus::ReportSyntaxError(const JSC::SourceCode & code, const JSC::ParserError &error) {
  std::ostringstream stream;
  try {
    auto provider  = code.provider();
    stream << std::endl;
    stream << "at " << provider->sourceOrigin().string().utf8().data() << ":" << error.line() << std::endl;
    stream << error.message().utf8().data() << std::endl;
    std::cerr << stream.str();
  } catch(const NX::Exception & e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
  }
}

NX::Context * NX::Nexus::mainContext() {
  return myMainContext;
}



