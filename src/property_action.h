/*
 * ====================================================================
 * Copyright (c) 2000 The Apache Software Foundation.  All rights
 * reserved.
 *
 * This software is licensed as described in the file LICENSE.txt,
 * which you should have received as part of this distribution.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://rapidsvn.tigris.org/.
 * ====================================================================
 */

#ifndef _PROPERTY_ACTION_H_INCLUDED_
#define _PROPERTY_ACTION_H_INCLUDED_

#include "action_thread.h"
#include "property_dlg.h"

class PropertyAction : public ActionThread
{
private:
  wxFrame *thisframe;
  const char * _target;
  PropertyDlg * propDialog;

public:
  PropertyAction (wxFrame * frame, apr_pool_t * __pool,
                  Tracer * tr, const char * target);

  void Perform ();
  void *Entry ();
};

#endif
