// $Id: rg_gaugeact.cc,v 3.0 2006-04-03 04:58:54 edwards Exp $
/*! \file
 *  \brief Generic RG style plaquette + rectangle gauge action
 */

#include "chromabase.h"
#include "actions/gauge/gaugeacts/rg_gaugeact.h"
#include "actions/gauge/gaugeacts/gaugeact_factory.h"
#include "actions/gauge/gaugeacts/gauge_createstate_aggregate.h"

namespace Chroma
{
 
  namespace RGGaugeActEnv 
  { 
    GaugeAction< multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> >* createGaugeAct(XMLReader& xml, 
											    const std::string& path) 
    {
      return new RGGaugeAct(CreateGaugeStateEnv::reader(xml, path), 
			    RGGaugeActParams(xml, path));
    }

    const std::string name = "RG_GAUGEACT";
    const bool registered = TheGaugeActFactory::Instance().registerObject(name, 
									  createGaugeAct);
  };


  RGGaugeActParams::RGGaugeActParams(XMLReader& xml_in, const std::string& path) {
    XMLReader paramtop(xml_in, path);

    try {
      read(paramtop, "beta", beta);
      read(paramtop, "c1", c1);
    }
    catch( const std::string& e ) { 
      QDPIO::cerr << "Error reading XML: " <<  e << endl;
      QDP_abort(1);
    }
  }

  void read(XMLReader& xml, const string& path, RGGaugeActParams& p) {
    RGGaugeActParams tmp(xml, path);
    p=tmp;
  }


  // Private initializer
  void
  RGGaugeAct::init(Handle< CreateGaugeState<P,Q> > cgs)
  {
    // Fold in normalizations and create action
    // Here, the rectangle weight is relative to the plaquette
    AnisoParam_t aniso;  // empty aniso
    Real coeff0 = beta;
    plaq = new PlaqGaugeAct(cgs,coeff0,aniso);

    Real coeff1 = beta * c1;
    rect = new RectGaugeAct(cgs,coeff1);
  } 

}

