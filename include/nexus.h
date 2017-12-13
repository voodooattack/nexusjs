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

#ifndef NEXUS_H
#define NEXUS_H

#ifdef BUILDING_WITH_CMAKE
#include <cmakeconfig.h>
#endif

#include <utility>
#include <list>
#include <string>

#include <boost/unordered_map.hpp>
#include <boost/program_options.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include "task.h"
#include "scheduler.h"
#include "context.h"
#include "exception.h"

namespace JSC {
  class ParserError;
}

namespace NX
{
  class Nexus: public boost::noncopyable
  {
  public:
    Nexus(int argc, const char ** argv);
    virtual ~Nexus();

    NX::Context * mainContext();

  public:
    void run();

    JSContextGroupRef group() { return myContextGroup; }
    NX::Scheduler * scheduler() { return myScheduler.get(); }
    const std::string & scriptPath() { return myScriptPath; }

    JSClassRef defineOrGetClass(const JSClassDefinition & def) {
      if (myClasses.find(def.className) != myClasses.end())
        return myClasses[def.className];
      return myClasses[def.className] = JSClassCreate(&def);
    }

    JSClassRef genericClass() { return defineOrGetClass({ 0, 0, "GenericObject" }); }

  protected:
    bool parseArguments();
    void initScheduler();
  public:
    static void ReportException(JSContextRef ctx, JSValueRef exception);
    static void ReportException(const std::exception & e);
    static void ReportSyntaxError(const JSC::SourceCode & code, const JSC::ParserError &error);
  private:
    Nexus ( const Nexus& other );
  protected:
    int argc;
    const char ** argv;
    std::list<std::string> myArguments;
    JSContextGroupRef myContextGroup;
    NX::Context * myMainContext;
    std::string myScriptSource;
    std::string myScriptPath;
    std::shared_ptr<NX::Scheduler> myScheduler;
    boost::program_options::variables_map myOptions;
    boost::unordered_map<std::string, JSClassRef> myClasses;
    int myExitStatus;
  };
}

#endif // NEXUS_H



