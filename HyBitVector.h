#ifndef HYBITVECTOR_H
#define HYBITVECTOR_H

#include "std_types.h"

typedef struct HyBitVector {
    uint8_t * _byte_pointer;
    int _cur_bit_index;
} HyBitVector, *pHyBitVector;


/*
 *  TODO
 *  1.detete the size of the bv's buffer
 *
 *
 */

/**
 *  @param m_this m_this pointer.
 *  @param byte_pointer the bv's buffer.
 *  @param offset the length of the offset for the bv's buffer.
 */
void setup_pointer(pHyBitVector m_this, void* byte_pointer, int offset);

/**
 *  @param m_this m_this pointer.
 *  @param scr_pointer src buffer
 *  @param offset src's bit-offset
 *  @param bit_size src's bit_size
 *
 *  @brief set_bits will move the bv's offset-cur
 */
void set_bits(pHyBitVector m_this, const void* src_pointer, int offset , int bit_size);

/**
 *  @param m_this m_this pointer.
 *  @param scr_pointer src buffer
 *  @param offset src's bit-offset
 *  @param bit_size src's bit_size
 *
 *  @brief set_bits will move the bv's offset-cur
 */
void set_bits_reverse(pHyBitVector m_this, const void* src_pointer, int offset , int bit_size);

/**
 *  @param m_this m_this pointer.
 *  @param bv_offset bv's bit-offset.
 *  @param des_pointer des' buffer.
 *  @param des_offset des' bit-offset.
 *  @param des_size des' size.
 *
 */
void get_bits(pHyBitVector m_this, int bv_offset, \
               void* des_pointer, int des_offset, int des_size);

#endif /* end of include guard: HYBITVECTOR_H */
