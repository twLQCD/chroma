// $Id: mres.cc,v 1.10 2005-02-07 04:15:19 edwards Exp $

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <cstdio>

#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "chroma.h"

using namespace Chroma;

struct Prop_t
{
  string          prop_file;
};

struct AppInput_t 
{
  multi1d<int> nrow;
  std::string  stateInfo;
  Cfg_t cfg;
  Prop_t prop;
};

//! To insure linking of code, place the registered code flags here
/*! This is the bit of code that dictates what fermacts are in use */
bool linkage_hack()
{
  bool foo = true;

  // All actions
  foo &= WilsonTypeFermActsEnv::registered;

  return foo;
}


void read(XMLReader& xml, const string& path, Prop_t& input)
{
  XMLReader inputtop(xml, path);
  read(inputtop, "prop_file", input.prop_file);
}


void read(XMLReader& xml, const string& path, AppInput_t& input)
{
  XMLReader inputtop(xml, path);

  // Read the input
  try
  {
    // The parameters holds the version number
    read(inputtop, "nrow", input.nrow);

    // Read in the gauge configuration info
    read(inputtop, "Cfg", input.cfg);

    // Read any auxiliary state information
    if( inputtop.count("StateInfo") == 1 ) {
      XMLReader xml_state_info(inputtop, "StateInfo");
      std::ostringstream os;
      xml_state_info.print(os);
      input.stateInfo = os.str();
    }
    else { 
      XMLBufferWriter s_i_xml;
      push(s_i_xml, "StateInfo");
      pop(s_i_xml);
      input.stateInfo = s_i_xml.str();
    }

    // Read in the source/propagator info
    read(inputtop, "Prop", input.prop);
  }
  catch (const string& e) 
  {
    QDPIO::cerr << "Error reading data: " << e << endl;
    throw;
  }
}



int main(int argc, char **argv)
{
  // Put the machine into a known state
  ChromaInitialize(&argc, &argv);

  AppInput_t input;
  XMLReader xml_in("./DATA");

  try {
    read(xml_in, "/mres", input);
  }
   catch( const string& e) { 
    QDPIO::cerr << "Caught Exception : " << e << endl;
    QDP_abort(1);
  }


  // Setup the lattice
  Layout::setLattSize(input.nrow);
  Layout::create();

  multi1d<LatticeColorMatrix> u(Nd);
  XMLReader gauge_file_xml, gauge_xml;
  gaugeStartup(gauge_file_xml, gauge_xml, u, input.cfg);

  // Read the prop

  LatticePropagator quark_propagator;
  ChromaProp_t prop_header;
  PropSource_t source_header;
  string       stateInfo;
  XMLReader prop_file_xml, prop_record_xml;

  {
    readQprop(prop_file_xml,
	      prop_record_xml,
	      quark_propagator,
	      input.prop.prop_file,
	      QDPIO_SERIAL);

    // Try to invert this record XML into a ChromaProp struct
    // Also pull out the id of this source
    try {
      read(prop_record_xml, "/Propagator/ForwardProp", prop_header);
      read(prop_record_xml, "/Propagator/PropSource", source_header);

      // Read any auxiliary state information
      if( prop_record_xml.count("/Propagator/StateInfo") == 1 ) 
      {
	XMLReader xml_state_info(prop_record_xml, "/Propagator/StateInfo");
	std::ostringstream os;
	xml_state_info.print(os);
	stateInfo = os.str();
      }
      else 
      { 
	// The user better have supplied something
	stateInfo = input.stateInfo;
      }
    }
    catch (const string& e) {
      QDPIO::cerr << "Error extracting forward_prop header: " << e << endl;
      throw;
    }
  }

  XMLFileWriter& xml_out = TheXMLOutputWriter::Instance();
  push(xml_out,"mres");

  proginfo(xml_out);    // Print out basic program info

  write(xml_out, "Input", xml_in);

  // Write out the config header
  write(xml_out, "Config_info", gauge_xml);

  // Write out the propagator header
  write(xml_out, "Propagator_file_info", prop_file_xml);
  write(xml_out, "Propagator_record_info", prop_record_xml);

  push(xml_out, "Output_version");
  write(xml_out, "out_version", 1);
  pop(xml_out);

  xml_out.flush();


  // Check if the gauge field configuration is unitarized
  unitarityCheck(u);

  // Calculate some gauge invariant observables just for info.
  Double w_plaq, s_plaq, t_plaq, link;
  MesPlq(u, w_plaq, s_plaq, t_plaq, link);

  push(xml_out, "Observables");
  write(xml_out, "w_plaq", w_plaq);
  write(xml_out, "s_plaq", s_plaq);
  write(xml_out, "t_plaq", t_plaq);
  write(xml_out, "link", link);
  pop(xml_out);


  // Basic parameters
  int j_decay = source_header.j_decay;
  multi1d<int> t_source = source_header.t_source;
  // Flags
  int t0      = t_source[j_decay];

  // Initialize the slow Fourier transform phases
  SftMom phases(0, true, j_decay);
  {
    multi1d<Double> forward_prop_corr = sumMulti(localNorm2(quark_propagator),
						 phases.getSet());
                                                                                
    push(xml_out, "Forward_prop_correlator");
    write(xml_out, "forward_prop_corr", forward_prop_corr);
    pop(xml_out);
  }


  //
  // Initialize fermion action
  //
  std::istringstream  xml_s(prop_header.fermact);
  XMLReader  fermacttop(xml_s);
  const string fermact_path = "/FermionAction";
  string fermact;

  try
  {
    read(fermacttop, fermact_path + "/FermAct", fermact);
  }
  catch (const std::string& e) 
  {
    QDPIO::cerr << "Error reading fermact: " << e << endl;
    throw;
  }

  QDPIO::cout << "FermAct = " << fermact << endl;
 
   // Make a reader for the stateInfo
  std::istringstream state_info_is(stateInfo);
  XMLReader state_info_xml(state_info_is);
  string state_info_path="/StateInfo";


  // 
  LatticePropagator delta_prop;
  LatticePropagator deltaSq_prop;

  try 
  { 
    QDPIO::cout << "Try generic WilsonTypeFermAct actions" << endl;

    // Generic Wilson-Type Array stuff
    FermionAction<LatticeFermion>* S_f =
      TheFermionActionFactory::Instance().createObject(fermact,
						       fermacttop,
						       fermact_path);
    
    Handle<const ConnectState> state(S_f->createState(u,
						      state_info_xml,
						      state_info_path)); 
    
    LinearOperator<LatticeFermion>* DelLs;

    // Possible actions
    const WilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >* S_dwf = 
      dynamic_cast<const WilsonTypeFermAct5D< LatticeFermion, multi1d<LatticeColorMatrix> >*>(S_f);

    const OverlapFermActBase* S_ov = dynamic_cast<const OverlapFermActBase*>(S_f);

    if (S_dwf != 0)
    {
      DelLs = const_cast<LinearOperator<LatticeFermion>*>(S_dwf->DeltaLs(state,prop_header.invParam));
    }
    else if (S_ov != 0)
    {
      DelLs = const_cast<LinearOperator<LatticeFermion>*>(S_ov->DeltaLs(state));
    }
    else
    {
      throw string("no suitable cast found");
    }
	
    for(int col = 0; col < Nc; col++) 
    {
      for(int spin = 0; spin < Ns; spin++) 
      {
	LatticeFermion tmp1, tmp2;

	// Move component from prop to ferm
	PropToFerm(quark_propagator, tmp1, col, spin);

	// Apply DeltaLs -> tmp2 = Delta_Ls tmp1
	(*DelLs)(tmp2, tmp1, PLUS);
	
	FermToProp(tmp2, delta_prop, col, spin);
      }
    }
    
    delete DelLs;
    delete S_f;
  }
  catch(const string& e) { 
    QDPIO::cout << "Error: " << e << endl;
  }
  catch(bad_cast) { 
    QDPIO::cout << "Action entered is not suitable to be cast to DWF " << endl;
  }


  multi1d<Double> pseudo_prop_corr = sumMulti(localNorm2(quark_propagator),
					      phases.getSet());

  multi1d<DComplex> delta_prop_corr = sumMulti(trace(adj(quark_propagator)*delta_prop),
					       phases.getSet());
  
  multi1d<DComplex> deltaSq_prop_corr = sumMulti(trace(adj(delta_prop)*delta_prop), 
						 phases.getSet());
   
  int length = pseudo_prop_corr.size();
  multi1d<Real> shifted_pseudo(length);
  multi1d<Real> shifted_delta(length);
  multi1d<Real> shifted_deltaSq(length);
  int t_src = t_source[j_decay];
  
  for(int t=0; t<length; t++){
    int t_eff( (t - t_src + length) % length ) ;
    shifted_pseudo[t_eff] = real(pseudo_prop_corr[t]);
    shifted_delta[t_eff]  = real(delta_prop_corr[t]);
    shifted_deltaSq[t_eff]= real(deltaSq_prop_corr[t]);
  }
  
  push(xml_out, "DeltaProp_correlator");
  write(xml_out, "delta_prop_corr", shifted_delta);
  pop(xml_out);
  
  push(xml_out, "DeltaSqProp_correlator");
  write(xml_out, "delta_sq_prop_corr", shifted_deltaSq);
  pop(xml_out);
  
  push(xml_out, "PsuedoPseudo_correlator");
  write(xml_out, "pseudo_prop_corr", shifted_pseudo);
  pop(xml_out);
  
  
  ChromaFinalize();
  exit(0);
}
