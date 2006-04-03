// -*- C++ -*-
// $Id: laplacian.h,v 3.0 2006-04-03 04:59:05 edwards Exp $
/*! \file
 *  \brief Laplacian smearing of a source
 */

#ifndef __laplacian_h__
#define __laplacian_h__

namespace Chroma {

//! Do a covariant Gaussian smearing of a lattice field
/*!
 * Arguments:
 *
 *  \param u        gauge field ( Read )
 *  \param chi      lattice color vector field ( Modify )
 *  \param j_decay  direction of decay ( Read )
 *  \param power    number of times to apply laplacian ( Read )
 */
void laplacian(const multi1d<LatticeColorMatrix>& u, 
	       LatticeColorVector& chi, 
	       int j_decay,
	       int power);


//! Do a covariant Gaussian smearing of a lattice field
/*!
 * Arguments:
 *
 *  \param u        gauge field ( Read )
 *  \param chi      lattice propagator field ( Modify )
 *  \param j_decay  direction of decay ( Read )
 *  \param power    number of times to apply laplacian ( Read )
 */
void laplacian(const multi1d<LatticeColorMatrix>& u, 
	       LatticePropagator& chi, 
	       int j_decay,
	       int power);

}  // end namespace Chroma

#endif
