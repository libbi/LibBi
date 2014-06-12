/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_STATE_MARGINALSIRSTATE_HPP
#define BI_STATE_MARGINALSIRSTATE_HPP

#include "MarginalMHState.hpp"

#include <vector>

namespace bi {
/**
 * State for MarginalSIR.
 *
 * @ingroup state
 *
 * @tparam B Model type.
 * @tparam L Location.
 * @tparam S1 Filter state type.
 * @tparam IO1 Output type.
 */
template<class B, Location L, class S1, class IO1>
class MarginalSIRState {
public:
  static const Location location = L;
  static const bool on_device = (L == ON_DEVICE);

  typedef MarginalMHState<B,L,S1,IO1> particle_type;

  typedef real value_type;
  typedef typename loc_vector<L,value_type>::type vector_type;
  typedef typename vector_type::vector_reference_type vector_reference_type;

  typedef int int_value_type;
  typedef typename loc_vector<L,int_value_type>::type int_vector_type;
  typedef typename int_vector_type::vector_reference_type int_vector_reference_type;

  /**
   * Constructor.
   *
   * @param m Model.
   * @param Ptheta Number of \f$\theta\f$-particles.
   * @param Px Number of \f$x\f$-particles.
   * @param T Number of time points.
   */
  MarginalSIRState(B& m, const int Ptheta = 0, const int Px = 0, const int T =
      0);

  /**
   * Shallow copy constructor.
   */
  MarginalSIRState(const MarginalSIRState<B,L,S1,IO1>& o);

  /**
   * Deep assignment operator.
   */
  MarginalSIRState& operator=(const MarginalSIRState<B,L,S1,IO1>& o);

  /**
   * Number of \f$\theta\f$-particles.
   */
  int size() const;

  /**
   * Log-weights vector.
   */
  vector_reference_type logWeights();

  /**
   * Log-weights vector.
   */
  const vector_reference_type logWeights() const;

  /**
   * Ancestors vector.
   */
  int_vector_reference_type ancestors();

  /**
   * Ancestors vector.
   */
  const int_vector_reference_type ancestors() const;

  /**
   * \f$\theta\f$-particles.
   */
  std::vector<particle_type*> thetas;

private:
  /**
   * Log-weights.
   */
  vector_type lws;

  /**
   * Log-evidences.
   */
  vector_type les;

  /**
   * Ancestors.
   */
  int_vector_type as;

  /**
   * Index of starting \f$\theta\f$-particle.
   */
  int ptheta;

  /**
   * Number of \f$\theta\f$-particles.
   */
  int Ptheta;

  /**
   * Serialize.
   */
  template<class Archive>
  void save(Archive& ar, const unsigned version) const;

  /**
   * Restore from serialization.
   */
  template<class Archive>
  void load(Archive& ar, const unsigned version);

  /*
   * Boost.Serialization requirements.
   */
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  friend class boost::serialization::access;
};
}

template<class B, bi::Location L, class S1, class IO1>
bi::MarginalSIRState<B,L,S1,IO1>::MarginalSIRState(B& m, const int Ptheta,
    const int Px, const int T) :
    thetas(Ptheta), lws(Ptheta), les(T), as(Ptheta), ptheta(0), Ptheta(Ptheta) {
  for (int p = 0; p < thetas.size(); ++p) {
    thetas[p] = new particle_type(Px, T);
  }
}

template<class B, bi::Location L, class S1, class IO1>
bi::MarginalSIRState<B,L,S1,IO1>::MarginalSIRState(
    const MarginalSIRState<B,L,S1,IO1>& o) :
    thetas(o.thetas.size()), lws(o.lws), les(o.les), as(o.as), ptheta(
        o.ptheta), Ptheta(o.Ptheta) {
  for (int p = 0; p < thetas.size(); ++p) {
    thetas[p] = new particle_type(*o.thetas[p]);
  }
}

template<class B, bi::Location L, class S1, class IO1>
bi::MarginalSIRState<B,L,S1,IO1>& bi::MarginalSIRState<B,L,S1,IO1>::operator=(
    const MarginalSIRState<B,L,S1,IO1>& o) {
  /* pre-condition */
  BI_ASSERT(o.size() == size());

  lws = o.lws;
  les = o.les;
  as = o.as;
  ptheta = o.ptheta;
  Ptheta = o.Ptheta;

  for (int p = 0; p < thetas.size(); ++p) {
    *thetas[p] = *o.thetas[p];
  }
  return *this;
}

template<class B, bi::Location L, class S1, class IO1>
int bi::MarginalSIRState<B,L,S1,IO1>::size() const {
  return Ptheta;
}

template<class B, bi::Location L, class S1, class IO1>
typename bi::MarginalSIRState<B,L,S1,IO1>::vector_reference_type bi::MarginalSIRState<
    B,L,S1,IO1>::logWeights() {
  return subrange(lws, ptheta, Ptheta);
}

template<class B, bi::Location L, class S1, class IO1>
const typename bi::MarginalSIRState<B,L,S1,IO1>::vector_reference_type bi::MarginalSIRState<
    B,L,S1,IO1>::logWeights() const {
  return subrange(lws, ptheta, Ptheta);
}

template<class B, bi::Location L, class S1, class IO1>
typename bi::MarginalSIRState<B,L,S1,IO1>::int_vector_reference_type bi::MarginalSIRState<
    B,L,S1,IO1>::ancestors() {
  return subrange(as, ptheta, Ptheta);
}

template<class B, bi::Location L, class S1, class IO1>
const typename bi::MarginalSIRState<B,L,S1,IO1>::int_vector_reference_type bi::MarginalSIRState<
    B,L,S1,IO1>::ancestors() const {
  return subrange(as, ptheta, Ptheta);
}

template<class B, bi::Location L, class S1, class IO1>
template<class Archive>
void bi::MarginalSIRState<B,L,S1,IO1>::save(Archive& ar,
    const unsigned version) const {
  save_resizable_vector(ar, version, lws);
  save_resizable_vector(ar, version, les);
  save_resizable_vector(ar, version, as);

  for (int p = 0; p < thetas.size(); ++p) {
    ar & *thetas[p];
  }
  ar & ptheta & Ptheta;
}

template<class B, bi::Location L, class S1, class IO1>
template<class Archive>
void bi::MarginalSIRState<B,L,S1,IO1>::load(Archive& ar,
    const unsigned version) {
  load_resizable_vector(ar, version, lws);
  load_resizable_vector(ar, version, les);
  load_resizable_vector(ar, version, as);

  for (int p = 0; p < thetas.size(); ++p) {
    ar & *thetas[p];
  }
  ar & ptheta & Ptheta;
}

#endif
