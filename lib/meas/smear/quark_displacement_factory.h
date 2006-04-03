// -*- C++ -*-
// $Id: quark_displacement_factory.h,v 3.0 2006-04-03 04:59:05 edwards Exp $
/*! \file
 *  \brief Factory for producing quark displacement objects
 */

#ifndef __quark_displacement_factory_h__
#define __quark_displacement_factory_h__

#include "singleton.h"
#include "objfactory.h"
#include "chromabase.h"
#include "meas/smear/quark_displacement.h"

namespace Chroma
{
  //! Quark displacement factory (foundry)
  typedef SingletonHolder< 
    ObjectFactory<QuarkDisplacement<LatticePropagator>, 
		  std::string,
		  TYPELIST_2(XMLReader&, const std::string&),
		  QuarkDisplacement<LatticePropagator>* (*)(XMLReader&,
							    const std::string&), StringFactoryError> >
  ThePropDisplacementFactory;


  //! Quark displacement factory (foundry)
  typedef SingletonHolder< 
    ObjectFactory<QuarkDisplacement<LatticeFermion>, 
		  std::string,
		  TYPELIST_2(XMLReader&, const std::string&),
		  QuarkDisplacement<LatticeFermion>* (*)(XMLReader&,
							 const std::string&), StringFactoryError> >
  TheFermDisplacementFactory;


  //! Quark displacement factory (foundry)
  typedef SingletonHolder< 
    ObjectFactory<QuarkDisplacement<LatticeColorVector>, 
		  std::string,
		  TYPELIST_2(XMLReader&, const std::string&),
		  QuarkDisplacement<LatticeColorVector>* (*)(XMLReader&,
							     const std::string&), StringFactoryError> >
  TheColorVecDisplacementFactory;

}


#endif
