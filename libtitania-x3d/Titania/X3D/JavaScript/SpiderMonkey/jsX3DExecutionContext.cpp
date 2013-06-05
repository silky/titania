/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstraße 31a, Leipzig, Germany 2011.
 *
 * All rights reserved. Holger Seelig <holger.seelig@yahoo.de>.
 *
 * THIS IS UNPUBLISHED SOURCE CODE OF create3000.
 *
 * The copyright notice above does not evidence any actual of intended
 * publication of such source code, and is an unpublished work by create3000.
 * This material contains CONFIDENTIAL INFORMATION that is the property of
 * create3000.
 *
 * No permission is granted to copy, distribute, or create derivative works from
 * the contents of this software, in whole or in part, without the prior written
 * permission of create3000.
 *
 * NON-MILITARY USE ONLY
 *
 * All create3000 software are effectively free software with a non-military use
 * restriction. It is free. Well commented source is provided. You may reuse the
 * source in any way you please with the exception anything that uses it must be
 * marked to indicate is contains 'non-military use only' components.
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 1999, 2012 Holger Seelig <holger.seelig@yahoo.de>.
 *
 * This file is part of the Titania Project.
 *
 * Titania is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3 only, as published by the
 * Free Software Foundation.
 *
 * Titania is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License version 3 for more
 * details (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version 3
 * along with Titania.  If not, see <http://www.gnu.org/licenses/gpl.html> for a
 * copy of the GPLv3 License.
 *
 * For Silvio, Joy and Adi.
 *
 ******************************************************************************/

#include "jsX3DExecutionContext.h"

#include "../../Bits/Error.h"
#include "../../Components/Core/X3DPrototypeInstance.h"
#include "../../Execution/Scene.h"
#include "Fields/jsMFNode.h"
#include "Fields/jsSFNode.h"
#include "jsProfileInfo.h"
#include "jsComponentInfoArray.h"
#include "jsExternProtoDeclarationArray.h"
#include "jsProtoDeclarationArray.h"
#include "jsRouteArray.h"
#include "jsString.h"

namespace titania {
namespace X3D {

JSClass jsX3DExecutionContext::static_class = {
	"X3DExecutionContext", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS

};

JSPropertySpec jsX3DExecutionContext::properties [ ] = {
	{ "specificationVersion", SPECIFICATION_VERSION, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, specificationVersion, NULL },
	{ "encoding",             ENCODING,              JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, encoding,             NULL },
	{ "worldURL",             WORLD_URL,             JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, worldURL,             NULL },

	{ "profile",              PROFILE,               JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, profile,     NULL },
	{ "components",           COMPONENTS,            JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, components,  NULL },

	{ "externprotos",         EXTERNPROTOS,          JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, externprotos, NULL },
	{ "protos",               PROTOS,                JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, protos,       NULL },
	{ "rootNodes",            ROOT_NODES,            JSPROP_ENUMERATE | JSPROP_SHARED | JSPROP_PERMANENT,                   rootNodes,    rootNodes },
	{ "routes",               ROUTES,                JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT, routes,       NULL },
	{ 0 }

};

JSFunctionSpec jsX3DExecutionContext::functions [ ] = {
	{ "createNode",  createNode,  1, 0 },
	{ "createProto", createProto, 1, 0 },

	{ "addRootNode",    addRootNode,    1, 0 },
	{ "removeRootNode", removeRootNode, 1, 0 },
	
	{ 0 }

};

void
jsX3DExecutionContext::initObject (JSContext* context, JSObject* object)
{
	JS_DefineProperties (context, object, properties);
	JS_DefineFunctions (context, object, functions);
}

JSBool
jsX3DExecutionContext::create (JSContext* context, X3DExecutionContext* executionContext, jsval* vp, const bool seal)
{
	JSObject* result = JS_NewObject (context, &static_class, NULL, NULL);

	if (result == NULL)
		return JS_FALSE;

	JS_SetPrivate (context, result, executionContext);

	initObject (context, result);

	//if (seal)
	//	JS_SealObject (context, result, JS_FALSE);

	*vp = OBJECT_TO_JSVAL (result);

	return JS_TRUE;
}

// Properties

JSBool
jsX3DExecutionContext::specificationVersion (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return JS_NewStringValue (context, executionContext -> getSpecificationVersion (), vp);
}

JSBool
jsX3DExecutionContext::encoding (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return JS_NewStringValue (context, executionContext -> getEncoding (), vp);
}

JSBool
jsX3DExecutionContext::worldURL (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return JS_NewStringValue (context, executionContext -> getWorldURL (), vp);
}

JSBool
jsX3DExecutionContext::profile (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));
	
	auto profile = executionContext -> getProfile ();

	if (profile)
		return jsProfileInfo::create (context, profile, vp);

	JS_SET_RVAL (context, vp, JSVAL_VOID);

	return JS_TRUE;
}

JSBool
jsX3DExecutionContext::components (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return jsComponentInfoArray::create (context, &executionContext -> getComponents (), vp);
}

JSBool
jsX3DExecutionContext::externprotos (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return jsExternProtoDeclarationArray::create (context, &executionContext -> getExternProtoDeclarations (), vp);
}

JSBool
jsX3DExecutionContext::protos (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return jsProtoDeclarationArray::create (context, &executionContext -> getProtoDeclarations (), vp);
}

JSBool
jsX3DExecutionContext::rootNodes (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	//	X3DExecutionContext* executionContext = (X3DExecutionContext*) JS_GetPrivate (context, obj);

	//	return jsMFNode::create (context, const_cast <X3DExecutionContext*> (&executionContext -> getRootNodes ()), vp, true);
	return JS_TRUE;
}

JSBool
jsX3DExecutionContext::rootNodes (JSContext* context, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
	//	X3DExecutionContext* executionContext = (X3DExecutionContext*) JS_GetPrivate (context, obj);
	//
	//	JSObject* omfnode;
	//
	//	if (not JS_ValueToObject (context, *vp, &omfnode))
	//		return JS_FALSE;
	//
	//	if (JS_GetClass (context, omfnode) not_eq jsMFNode::getClass ())
	//	{
	//		JS_ReportError (context, "Type of argument is invalid - should be MFNode, is %s", JS_GetClass (context, omfnode) -> name);
	//		return JS_FALSE;
	//	}
	//
	//	MFNode* mfnode = static_cast <MFNode*> (JS_GetPrivate (context, omfnode));
	//
	//	executionContext -> setRootNodes (*mfnode);

	return JS_TRUE;
}

JSBool
jsX3DExecutionContext::routes (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, obj));

	return jsRouteArray::create (context, &executionContext -> getRoutes (), vp);
}

// Functions

JSBool
jsX3DExecutionContext::createNode (JSContext* context, uintN argc, jsval* vp)
{
	if (argc == 1)
	{
		try
		{
			JSString* name;

			jsval* argv = JS_ARGV (context, vp);

			if (not JS_ConvertArguments (context, argc, argv, "S", &name))
				return JS_FALSE;

			auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, JS_THIS_OBJECT (context, vp)));
			auto node             = executionContext -> createNode (JS_GetString (context, name));

			node -> setup ();

			return jsSFNode::create (context, new SFNode <X3DBaseNode> (node), &JS_RVAL (context, vp));
		}
		catch (const X3DError & exception)
		{
			JS_ReportError (context, exception .what ());
			return JS_FALSE;
		}
	}

	JS_ReportError (context, "wrong number of arguments");

	return JS_FALSE;
}

JSBool
jsX3DExecutionContext::createProto (JSContext* context, uintN argc, jsval* vp)
{
	if (argc == 1)
	{
		try
		{
			JSString* name;

			jsval* argv = JS_ARGV (context, vp);

			if (not JS_ConvertArguments (context, argc, argv, "S", &name))
				return JS_FALSE;

			auto executionContext = static_cast <X3DExecutionContext*> (JS_GetPrivate (context, JS_THIS_OBJECT (context, vp)));
			auto node             = executionContext -> createProtoInstance (JS_GetString (context, name));

			node -> setup ();

			return jsSFNode::create (context, new SFNode <X3DBaseNode> (*node), &JS_RVAL (context, vp));
		}
		catch (const X3DError & exception)
		{
			JS_ReportError (context, exception .what ());
			return JS_FALSE;
		}
	}

	JS_ReportError (context, "wrong number of arguments");

	return JS_FALSE;
}

JSBool
jsX3DExecutionContext::addRootNode (JSContext* context, uintN argc, jsval* vp)
{
	if (argc == 1)
	{
		*vp = JSVAL_VOID;

		return JS_TRUE;
	}

	JS_ReportError (context, "wrong number of arguments");

	return JS_FALSE;
}

JSBool
jsX3DExecutionContext::removeRootNode (JSContext* context, uintN argc, jsval* vp)
{
	if (argc == 1)
	{
		*vp = JSVAL_VOID;

		return JS_TRUE;
	}

	JS_ReportError (context, "wrong number of arguments");

	return JS_FALSE;
}

} // X3D
} // titania
