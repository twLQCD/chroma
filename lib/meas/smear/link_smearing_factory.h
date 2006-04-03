// -*- C++ -*-
// $Id: link_smearing_factory.h,v 3.0 2006-04-03 04:59:05 edwards Exp $
/*! \file
 *  \brief Factory for producing link smearing objects
 */

#ifndef __link_smearing_factory_h__
#define __link_smearing_factory_h__

#include "singleton.h"
#include "objfactory.h"
#include "chromabase.h"
#include "meas/smear/link_smearing.h"

namespace Chroma
{
  //! Link smearing factory (foundry)
  /*! @ingroup smear */
  typedef SingletonHolder< 
    ObjectFactory<LinkSmearing,
		  std::string,
		  TYPELIST_2(XMLReader&, const std::string&),
		  LinkSmearing* (*)(XMLReader&,
				    const std::string&), StringFactoryError> >
  TheLinkSmearingFactory;



  //! Convenience function to smear link
  /*! @ingroup smear */
  void linkSmear(multi1d<LatticeColorMatrix>& u, const std::string& path,
		 const std::string& link_xml, const std::string& link_type);

}


#endif
