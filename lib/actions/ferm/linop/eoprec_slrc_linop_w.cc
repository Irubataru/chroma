// $Id: eoprec_slrc_linop_w.cc,v 3.2 2007-10-09 03:31:14 edwards Exp $
/*! \file
 *  \brief Even-odd preconditioned Clover linear operator (fat-relevant, thin-irrelevant terms)
 *
 * Here, the relevant terms are smeared and the irrelevant terms are not smeared.
 * Code provided by Thomas Kaltenbrunner.
 *
 */

#include "actions/ferm/linop/eoprec_slrc_linop_w.h"

using namespace QDP::Hints;

namespace Chroma 
{ 

  //! Creation routine with Anisotropy
  /*!
   * \param u_ 	    gauge field     	       (Read)
   * \param param_  fermion kappa   	       (Read)
   */
  void EvenOddPrecSLRCLinOp::create(Handle< FermState<T,P,Q> > fs, 
				    const CloverFermActParams& param_)
  {
    START_CODE();

    param = param_;

    // Need to make sure that fs is a stout ferm state
    // We want to have Clover with thin links
    thin_fs  = new SimpleFermState<T,P,Q>( fs->getFermBC(), (fs.cast<SLICFermState<T, P, Q> >())->getThinLinks());
    clov.create(thin_fs, param);

    invclov.create(thin_fs,param,clov);  // make a copy
    invclov.choles(0);  // invert the cb=0 part

    //...and WilsonDslash with stout links
    D.create(fs, param.anisoParam);
    
    END_CODE();
  }

  //! Apply the the odd-odd block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::oddOddLinOp(LatticeFermion& chi, const LatticeFermion& psi, 
				      enum PlusMinus isign) const
  {
    clov.apply(chi, psi, isign, 1);
  }


  //! Apply the the even-even block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::evenEvenLinOp(LatticeFermion& chi, const LatticeFermion& psi, 
					enum PlusMinus isign) const
  {
    // Nuke for testing
    clov.apply(chi, psi, isign, 0);
  }

  //! Apply the inverse of the even-even block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::evenEvenInvLinOp(LatticeFermion& chi, const LatticeFermion& psi, 
					   enum PlusMinus isign) const
  {
    invclov.apply(chi, psi, isign, 0);
  }
  

  //! Apply even-odd linop component
  /*!
   * The operator acts on the entire even sublattice
   *
   * \param chi 	  Pseudofermion field     	       (Write)
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void 
  EvenOddPrecSLRCLinOp::evenOddLinOp(LatticeFermion& chi, 
				       const LatticeFermion& psi, 
				       enum PlusMinus isign) const
  {
    START_CODE();

    Real mhalf = -0.5;

    D.apply(chi, psi, isign, 0);
    chi[rb[0]] *= mhalf;
  
    END_CODE();
  }

  //! Apply odd-even linop component
  /*!
   * The operator acts on the entire odd sublattice
   *
   * \param chi 	  Pseudofermion field     	       (Write)
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void 
  EvenOddPrecSLRCLinOp::oddEvenLinOp(LatticeFermion& chi, 
				       const LatticeFermion& psi, 
				       enum PlusMinus isign) const
  {
    START_CODE();

    Real mhalf = -0.5;

    D.apply(chi, psi, isign, 1);
    chi[rb[1]] *= mhalf;
  
    END_CODE();
  }


  //! Apply even-odd preconditioned Clover fermion linear operator
  /*!
   * \param chi 	  Pseudofermion field     	       (Write)
   * \param psi 	  Pseudofermion field     	       (Read)
   * \param isign   Flag ( PLUS | MINUS )   	       (Read)
   */
  void EvenOddPrecSLRCLinOp::operator()(LatticeFermion & chi, 
					  const LatticeFermion& psi, 
					  enum PlusMinus isign) const
  {
    START_CODE();

    LatticeFermion tmp1; moveToFastMemoryHint(tmp1);
    LatticeFermion tmp2; moveToFastMemoryHint(tmp2);
    Real mquarter = -0.25;
  
    //  tmp1_o  =  D_oe   A^(-1)_ee  D_eo  psi_o
    D.apply(tmp1, psi, isign, 0);
    invclov.apply(tmp2, tmp1, isign, 0);
    D.apply(tmp1, tmp2, isign, 1);

    //  chi_o  =  A_oo  psi_o  -  tmp1_o
    clov.apply(chi, psi, isign, 1);

    chi[rb[1]] += mquarter*tmp1;
    
    END_CODE();
  }


  //! Apply the even-even block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::derivEvenEvenLinOp(multi1d<LatticeColorMatrix>& ds_u, 
					     const LatticeFermion& chi, const LatticeFermion& psi, 
					     enum PlusMinus isign) const
  {
    START_CODE();

    ds_u.resize(Nd);
    clov.deriv(ds_u, chi, psi, isign, 0);

    //Clover is thin, so no fatForceToThin needed, only BCs have to be changed

    // Undo ferm boundaries on ds_U
    slrc_fs->getFermBC()->modify(ds_u);
    // But reinforce gauge boundaries
    slrc_fs->getFermBC()->zero(ds_u);
    
    END_CODE();
  }

  //! Apply the even-even block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::derivLogDetEvenEvenLinOp(multi1d<LatticeColorMatrix>& ds_u,
						 enum PlusMinus isign) const
  {
    START_CODE();
    
    ds_u.resize(Nd);
    invclov.derivTrLn(ds_u, isign, 0);

    //Clover is thin, so no fatForceToThin needed, only BCs have to be changed

    // Undo ferm boundaries on ds_U
    slrc_fs->getFermBC()->modify(ds_u);
    // But reinforce gauge boundaries
    slrc_fs->getFermBC()->zero(ds_u);
    
    END_CODE();
  }

  //! Apply the the even-odd block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::derivEvenOddLinOp(multi1d<LatticeColorMatrix>& ds_u, 
					    const LatticeFermion& chi, const LatticeFermion& psi, 
					    enum PlusMinus isign) const
  {
    START_CODE();
 
    //temp variable for the fat force
    multi1d<LatticeColorMatrix> ds_tmp(Nd);
    ds_u.resize(Nd);
 
    D.deriv(ds_tmp, chi, psi, isign, 0);

    //WilsonDslash is thick, so need fatForceToThin here! (includes change of BCs)
    SLICFermState<T, P, Q>& sfs = dynamic_cast<SLICFermState<T,P,Q>& >(*slrc_fs);
    sfs.fatForceToThin(ds_tmp,ds_u);

    // finally: WilsonDslash as implemented here comes with a weight factor of -1/2 in the action
    for(int mu=0; mu < Nd; mu++) { 
      ds_u[mu]  *= Real(-0.5);
    }
    END_CODE();
  }
 
  //! Apply the the odd-even block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::derivOddEvenLinOp(multi1d<LatticeColorMatrix>& ds_u, 
					    const LatticeFermion& chi, const LatticeFermion& psi, 
					    enum PlusMinus isign) const
  {
    START_CODE();

    //temp variable for the fat force
    multi1d<LatticeColorMatrix> ds_tmp(Nd);
    ds_u.resize(Nd);

    D.deriv(ds_tmp, chi, psi, isign, 1);

    //WilsonDslash is thick, so need fatForceToThin here! (includes change of BCs)
    SLICFermState<T, P, Q>& sfs = dynamic_cast<SLICFermState<T,P,Q>& >(*slrc_fs);
    sfs.fatForceToThin(ds_tmp,ds_u);

    // finally: WilsonDslash as implemented here comes with a weight factor of -1/2 in the action
    for(int mu=0; mu < Nd; mu++) { 
      ds_u[mu]  *= Real(-0.5);
    }
    END_CODE();
  }

  // Inherit this
  //! Apply the the odd-odd block onto a source vector
  void 
  EvenOddPrecSLRCLinOp::derivOddOddLinOp(multi1d<LatticeColorMatrix>& ds_u, 
					   const LatticeFermion& chi, const LatticeFermion& psi, 
					   enum PlusMinus isign) const
  {   
    START_CODE();

    ds_u.resize(Nd);
    clov.deriv(ds_u, chi, psi, isign, 1);

    //Clover is thin, so no fatForceToThin needed, only BCs have to be changed

    // Undo ferm boundaries on ds_U
    slrc_fs->getFermBC()->modify(ds_u);
    // But reinforce gauge boundaries
    slrc_fs->getFermBC()->zero(ds_u);
  
    END_CODE();
  }


  //! Return flops performed by the operator()
  unsigned long EvenOddPrecSLRCLinOp::nFlops() const
  {
    unsigned long cbsite_flops = 2*D.nFlops()+2*clov.nFlops()+4*Nc*Ns;
    return cbsite_flops*(Layout::sitesOnNode()/2);
  }

  //! Get the log det of the even even part
  // BUt for now, return zero for testing.
  Double EvenOddPrecSLRCLinOp::logDetEvenEvenLinOp(void) const  {
    return invclov.cholesDet(0);
  }
} // End Namespace Chroma
