/*
 * opencog/atoms/execution/DLScheme.cc
 *
 * Copyright (C) 2019 Linas Vepstas
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <dlfcn.h>
#include <opencog/util/exceptions.h>
#include "DLScheme.h"

using namespace opencog;

static void* library = nullptr;
SchemeEval* opencog::get_evaluator_for_scheme(AtomSpace* as)
{
	if (nullptr == library) library = dlopen("libsmob.so", RTLD_LAZY);
	if (nullptr == library)
		throw RuntimeException(TRACE_INFO,
			"Unable to dynamically load libsmob.so: %s",
			dlerror());

	static void* getev = dlsym(library, "get_scheme_evaluator");
	if (nullptr == getev)
		throw RuntimeException(TRACE_INFO,
			"Unable to dynamically load scheme evaluator: %s",
			dlerror());

	typedef SchemeEval* (*SEGetter)(AtomSpace*);

	// static SEGetter getter = std::reinterpret_cast<SEGetter>(getev);
	static SEGetter getter = (SEGetter) getev;

	return getter(as);
}

static __attribute__ ((destructor)) void fini(void)
{
	if (library) dlclose(library);
}
