#include "PythonVector.h"
#include <sstream>
#include <string>

void copy_array(const double * src, double * dst, const unsigned size) {
  msg_assert(src && dst, "Not initialized memory");
  for (unsigned i{}; i < size; ++i)
    dst[i] = src[i];
}

void copy_array(const double * src, const unsigned src_size, double * dst, const unsigned dst_size, int src_start_id, int src_end_id, int dst_start_id = 0) {
  msg_assert(src && dst, "Not initialized memory");
  msg_assert(src_end_id <= src_size, "Out of src array range");
  msg_assert(dst_start_id + src_end_id - src_start_id <= dst_size, "Out of dst array range");
  
  for (unsigned i{}; i < src_end_id - src_start_id; ++i)
    dst[dst_start_id + i] = src[src_start_id + i];
}

PythonVector::PythonVector(unsigned size) : m_size{size} {
  m_array = new double[m_size];
  msg_assert(m_array, "Out of memory");
}

PythonVector::PythonVector(const double * array, unsigned size) : m_size{size} {
  m_array = new double[m_size];
  msg_assert(m_array, "Out of memory");
  
  copy_array(array, m_array, m_size);
}

PythonVector::PythonVector(const PythonVector & pv) {
  m_size = pv.get_size();
  m_array = new double[m_size];
  msg_assert(m_array, "Out of memory");
  
  copy_array(pv.m_array, m_array, m_size);
}

PythonVector & PythonVector::operator=(const PythonVector & pv) {
  if (this == &pv)
    return *this;
  delete[] m_array;
  m_size = pv.get_size();
  m_array = new double[m_size];
  msg_assert(m_array, "Out of memory");
  
  copy_array(pv.m_array, m_array, m_size);
  
  return *this;
}

PythonVector::~PythonVector() {
  delete[] m_array;
  m_array = nullptr;
  m_size = 0;
}

void PythonVector::fill_array(const double * src, const unsigned size) {
  msg_assert(size == m_size, "Array sizes are not equal");
  copy_array(src, m_array, m_size);
}

PythonVector PythonVector::range(int n, int m) const {
  msg_assert(n < m, "n should be less than m");
  msg_assert(m <= m_size, "m should be less than m_size");
  
  const int res_size = m - n;
  double * res = new double[res_size];
  msg_assert(res, "Out of memory");
  
  copy_array(m_array, m_size, res, res_size, n, m);

  return PythonVector(res, res_size);
}

PythonVector::operator std::string() const {
  std::string str;
  
  str += "{";
  for (unsigned i{}; i < m_size; ++i) {
    if (i != 0)
      str += " ";
    std::ostringstream os;
    os << m_array[i];
    str += os.str();
    if (i != m_size - 1)
      str += ",";
  }
  str += "}";
  
  return str;
}

double PythonVector::operator[](int idx) const {
  if (!(idx % m_size))
    return m_array[0];
  
  if (idx >= 0)
    return m_array[idx % m_size];
  else
    return m_array[m_size - std::abs(idx) % m_size];
}

PythonVector PythonVector::operator+(const PythonVector & pv) const {
  const unsigned res_size = m_size + pv.m_size;
  double * res = new double[res_size];
  msg_assert(res, "Out of memory");
  
  copy_array(m_array, m_size, res, res_size, 0, m_size);
  copy_array(pv.m_array, pv.m_size, res, res_size, 0, pv.m_size, m_size);
  
  return PythonVector(res, res_size);
}

PythonVector PythonVector::operator*(const PythonVector & pv) const {
  const unsigned res_size = m_size * pv.m_size;
  double * res = new double[res_size];
  msg_assert(res, "Out of memory");

  for (unsigned i{}; i < pv.m_size; ++i)
    for (unsigned j{}; j < m_size; ++j)
      res[i * m_size + j] = pv.m_array[i] * m_array[j];

  return PythonVector(res, res_size);
}

std::ostream & operator<<(std::ostream & os, const PythonVector & pv) {
  os << "PythonVector(" << pv.m_size << "): ";
  for (unsigned i{}; i < pv.m_size; ++i)
    os << pv.m_array[i] << " ";
  os << std::endl;
  return os;
}