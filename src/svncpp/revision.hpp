/*
 * ====================================================================
 * Copyright (c) 2002 The RapidSvn Group.  All rights reserved.
 *
 * This software is licensed as described in the file LICENSE.txt,
 * which you should have received as part of this distribution.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://rapidsvn.tigris.org/.
 * ====================================================================
 */

#ifndef _SVNCPP_REVISION_HPP_
#define _SVNCPP_REVISION_HPP_ 

// subversion includes
#include "svn_types.h"
#include "svn_opt.h"

// forward declarations
//typedef struct svn_opt_revision_t;
//typedef struct svn_revnum_t;

namespace svn
{
  /**
   * Class that encapsulates svn_opt_revnum_t.
   *
   * @see svn_opt_revnum_t
   */
  class Revision
  {
  private:
    svn_opt_revision_t m_revision;

    void
    init (const svn_opt_revision_t revision);

  public:
    /**
     * Constructor
     *
     * @param revision revision information
     */
    Revision (const svn_opt_revision_t revision);

    /**
     * Constructor
     *
     * @param revnum revision number
     */
    Revision (const svn_revnum_t revnum);

    /**
     * Constructor
     *
     * @param kind
     */
    Revision (const svn_opt_revision_kind kind);

    /**
     * Constructor
     *
     * @param date 
     */
    Revision (const apr_time_t date);

    /**
     * Copy constructor
     *
     * @param revision Source
     */
    Revision (const Revision & revision);

    /**
     * @return revision information
     */
    const svn_opt_revision_t 
    revision () const;

    /**
     * @return revision numver
     */
    const svn_revnum_t 
    revnum () const;

    /**
     * @return revision kind
     */
    const svn_opt_revision_kind 
    kind () const;

    /**
     * @return date
     */
    const apr_time_t
    date () const;
  };
}

#endif
/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "../../rapidsvn-dev.el")
 * end:
 */