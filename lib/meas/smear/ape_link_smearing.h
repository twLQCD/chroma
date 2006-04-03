// -*- C++ -*-
// $Id: ape_link_smearing.h,v 3.0 2006-04-03 04:59:04 edwards Exp $
/*! \file
 *  \brief APE link smearing
 */

#ifndef __ape_link_smearing_h__
#define __ape_link_smearing_h__

#include "meas/smear/link_smearing.h"

namespace Chroma
{

  //! Name and registration
  namespace APELinkSmearingEnv
  {
    extern const std::string name;
    extern const bool registered;
  
    //! Params for APE link smearing
    /*! @ingroup smear */
    struct Params
    {
      Params() {}
      Params(XMLReader& in, const std::string& path);
      void writeXML(XMLWriter& in, const std::string& path) const;
    
      int link_smear_num;               /*!< Smearing hits */
      Real link_smear_fact;		/*!< Smearing parameters */
      int no_smear_dir;			/*!< Direction to not smear */
    };


    //! APE link smearing
    /*! @ingroup smear
     *
     * APE link smearing object
     */
    class LinkSmear : public LinkSmearing
    {
    public:
      //! Full constructor
      LinkSmear(const Params& p) : params(p) {}

      //! Smear the links
      void operator()(multi1d<LatticeColorMatrix>& u) const;

    private:
      //! Hide partial constructor
      LinkSmear() {}

    private:
      Params  params;   /*!< smearing params */
    };

  }  // end namespace


  //! Reader
  /*! @ingroup smear */
  void read(XMLReader& xml, const string& path, APELinkSmearingEnv::Params& param);

  //! Writer
  /*! @ingroup smear */
  void write(XMLWriter& xml, const string& path, const APELinkSmearingEnv::Params& param);

}  // end namespace Chroma


#endif
