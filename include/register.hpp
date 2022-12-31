#pragma once
/**
From https://github.com/kensmith/cortex-from-scratch/blob/master/reg_t.hpp
*/

/*
int __builtin_ctzl (unsigned int x)
   Returns the number of trailing 0-bits in x,
   starting at the most significant bit position.
   If x is 0, the result is undefined.
*/

#include "common_defs.hpp"

namespace adc {

//! \cond internal
/**
 * @brief Register namespace for register access
 *
 */
namespace reg {

template <template <typename> class mutability_policy_t,
          typename access_policy_t, address_t address, address_t mask,
          address_t offset = __builtin_ctzl(mask)>
struct reg_t {
  /**
   * Read the subregister.
   * @return the value
   */
  static value_t read() {
    return mutability_policy_t<access_policy_t>::read(
        reinterpret_cast<volatile address_t *>(address), mask, offset);
  }

  /**
   * Write a subregister.
   * @param value the new value
   */
  static void write(value_t value) {
    mutability_policy_t<access_policy_t>::write(
        reinterpret_cast<volatile address_t *>(address), mask, offset, value);
  }

  /**
   * Set all bits in the subregister to one.
   */
  static void set() {
    mutability_policy_t<access_policy_t>::set(
        reinterpret_cast<volatile address_t *>(address), mask);
  }

  /**
   * Clear all bits in the subregister to zero.
   */
  static void clear() {
    mutability_policy_t<access_policy_t>::clear(
        reinterpret_cast<volatile address_t *>(address), mask);
  }
};

/**
 * @brief Direct access access policy, always available, unlike bitbanding.
 *
 */
struct direct_access_t {
  /**
   * @brief Read value from mask in address at offset
   * @param address
   * @param mask
   * @param offset
   * @return value
   */
  static value_t read(volatile address_t *address, address_t mask,
                      address_t offset) {
    return (*address & mask) >> offset;
  }

  /**
   * @brief Write value to mask in address at offset
   * @param address
   * @param mask
   * @param offset
   * @param value
   */
  static void write(volatile address_t *address, address_t mask,
                    address_t offset, value_t value) {
    *address = (*address & ~mask) | ((value << offset) & mask);
  }

  /**
   * @brief Clear bits for mask in address
   *
   * @param address
   * @param mask
   */
  static void clear(volatile address_t *address, address_t mask) {
    *address &= ~mask;
  }

  /**
   * @brief Set bits for mask in address
   *
   * @param address
   * @param mask
   */
  static void set(volatile address_t *address, address_t mask) {
    *address = mask;
  }
};

/**
 * A read-only mutability policy for use with reg_t.
 */
template <typename access_policy_t> struct ro_t {
  static value_t read(volatile address_t *address, address_t mask,
                      address_t offset) {
    return access_policy_t::read(address, mask, offset);
  }
};

/**
 * A read-write mutability policy for use with reg_t.
 */
template <typename access_policy_t> struct rw_t : ro_t<access_policy_t> {
  static void write(volatile address_t *address, address_t mask,
                    address_t offset, value_t value) {
    access_policy_t::write(address, mask, offset, value);
  }

  static void set(volatile address_t *address, address_t mask) {
    access_policy_t::set(address, mask);
  }

  static void clear(volatile address_t *address, address_t mask) {
    access_policy_t::clear(address, mask);
  }
};

} // namespace reg
//! \endcond

}; // namespace adc
