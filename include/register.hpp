/**
From https://github.com/kensmith/cortex-from-scratch/blob/master/reg_t.hpp
*/

/*
int __builtin_ctzl (unsigned int x)
   Returns the number of trailing 0-bits in x,
   starting at the most significant bit position.
   If x is 0, the result is undefined.
*/

#include "Arduino.h"

using address_t = uint32_t;
using value_t = uint32_t;

template <typename mutability_policy_t, address_t address, address_t mask,
          address_t offset = __builtin_ctzl(mask)>
struct reg_t {
  /**
   * Read the subregister.
   * @return the value
   */
  static value_t read() {
    return mutability_policy_t::read(
        reinterpret_cast<volatile address_t *>(address), mask, offset);
  }

  /**
   * Write a subregister.
   * @param value the new value
   */
  static void write(value_t value) {
    mutability_policy_t::write(reinterpret_cast<volatile address_t *>(address),
                               mask, offset, value);
  }

  /**
   * Set all bits in the subregister to one.
   */
  static void set() {
    mutability_policy_t::set(reinterpret_cast<volatile address_t *>(address),
                             mask);
  }

  /**
   * Clear all bits in the subregister to zero.
   */
  static void clear() {
    mutability_policy_t::clear(reinterpret_cast<volatile address_t *>(address),
                               mask);
  }
};

/**
 * @brief Direct access atomic policy, always available, unlike bitbanding.
 *
 */
struct direct_access_t {
  static value_t read(volatile address_t *address, address_t mask,
                      address_t offset) {
    return (*address & mask) >> offset;
  }

  static void write(volatile address_t *address, address_t mask,
                    address_t offset, value_t value) {
    *address = (*address & ~mask) | ((value << offset) & mask);
  }

  static void clear(volatile address_t *address, address_t mask) {
    *address &= ~mask;
  }

  static void set(volatile address_t *address, address_t mask) {
    *address = mask;
  }
};

using default_access_policy_t = direct_access_t;

/**
 * A read-only mutability policy for use with reg_t.
 */
template <typename access_policy_t = default_access_policy_t> struct ro_t {
  static value_t read(volatile address_t *address, address_t mask,
                      address_t offset) {
    return access_policy_t::read(address, mask, offset);
  }
};

/**
 * A read-write mutability policy for use with reg_t.
 */
template <typename access_policy_t = default_access_policy_t>
struct rw_t : ro_t<access_policy_t> {
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
