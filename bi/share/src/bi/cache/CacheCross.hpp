/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_CACHE_CROSSCACHE_HPP
#define BI_CACHE_CROSSCACHE_HPP

#include "Cache.hpp"
#include "../math/loc_matrix.hpp"

namespace bi {
/**
 * Cached matrix, where rows are writable and columns are readable. Useful
 * for stacking discontiguous writes in memory to later ensure a contiguous
 * write to disk.
 *
 * @ingroup io_cache
 *
 * @tparam T1 Scalar type.
 * @tparam CL Cache location.
 */
template<class T1, Location CL>
class CacheCross : public Cache {
public:
  /**
   * Matrix type.
   */
  typedef typename loc_matrix<CL,T1>::type matrix_type;

  /**
   * Matrix reference type.
   */
  typedef typename matrix_type::matrix_reference_type matrix_reference_type;

  /**
   * Vector reference type.
   */
  typedef typename matrix_type::vector_reference_type vector_reference_type;

  /**
   * Constructor.
   *
   * @param rows Number of rows.
   * @param cols Number of columns.
   */
  CacheCross(const int rows = 0, const int cols = 0);

  /**
   * Read row.
   *
   * @param i Index of row.
   *
   * @return Row range.
   */
  const vector_reference_type get(const int i) const;

  /**
   * Read rows.
   *
   * @param i Index of first row.
   * @param len Number of rows.
   *
   * @return Row range.
   */
  const matrix_reference_type get(const int i, const int len) const;

  /**
   * Write row.
   *
   * @tparam V1 Vector type.
   *
   * @param i Row index.
   * @param x Rows.
   */
  template<class V1>
  void set(const int i, const V1 x);

  /**
   * Resize cache.
   */
  void resize(const int rows, const int cols);

  /**
   * Empty cache.
   */
  void empty();

private:
  /**
   * Contents of cache.
   */
  matrix_type X;
};
}

template<class T1, bi::Location CL>
inline bi::CacheCross<T1,CL>::CacheCross(const int rows, const int cols) :
    Cache(rows), X(rows, cols) {
  //
}

template<class T1, bi::Location CL>
inline const typename bi::CacheCross<T1,CL>::vector_reference_type bi::CacheCross<T1,CL>::get(
    const int i) const {
  /* pre-condition */
  BI_ASSERT(isValid(i));

  return row(X, i);
}

template<class T1, bi::Location CL>
inline const typename bi::CacheCross<T1,CL>::matrix_reference_type bi::CacheCross<T1,CL>::get(
    const int i, const int len) const {
  /* pre-condition */
  BI_ASSERT(isValid(i, len));

  return rows(X, i, len);
}

template<class T1, bi::Location CL>
template<class V1>
inline void bi::CacheCross<T1,CL>::set(const int p, const V1 x) {
  /* pre-condition */
  BI_ASSERT(p >= 0 && p < size());

  const int len = x.size();
  row(X, p) = x;
  setDirty(p, true);
  setValid(p, true);
}

template<class T1, bi::Location CL>
void bi::CacheCross<T1,CL>::resize(const int rows, const int cols) {
  X.resize(rows, cols, true);
  Cache::resize(rows);
}

template<class T1, bi::Location CL>
void bi::CacheCross<T1,CL>::empty() {
  X.resize(0,0);
  Cache::empty();
}

#endif