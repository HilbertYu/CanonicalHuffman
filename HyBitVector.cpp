#include "HyBitVector.h"


void  get_bits(pHyBitVector m_this, int bv_offset, \
               void* des_pointer, int des_offset, int des_size)
{
    uint8_t * des_ptr = (uint8_t *) des_pointer;
    int bv_byte_index = 0;
    int bv_bit_index = 0;
    int des_byte_index = 0;
    int des_bit_index = 0;

    int i = 0;

    for (i = 0; i < des_size; ++i) {
        bv_byte_index = (bv_offset + i) >> 3;
        bv_bit_index = (bv_offset + i) & 0x07;
        des_byte_index = (des_offset + i) >> 3;
        des_bit_index = (des_offset + i) & 0x07;

        if ((m_this->_byte_pointer[bv_byte_index] >> bv_bit_index) & 0x01) {
            des_ptr[des_byte_index] |= (0x01 << des_bit_index);
        }
        else {
            des_ptr[des_byte_index] &= ~(0x01 << des_bit_index);
        }
    }

}


void setup_pointer(pHyBitVector m_this, void * byte_pointer, int offset) {
    m_this->_byte_pointer = (uint8_t*) byte_pointer;
    m_this->_cur_bit_index = offset;
//    m_this->_bit_size = bit_size;
}


void set_bits(pHyBitVector m_this, const void* src_pointer, int offset, int bit_size) {
    const uint8_t * src_ptr = (uint8_t * ) src_pointer;

    int i = 0;
    int byte_offset = 0;
    int bit_offset = offset;

    int bv_byte_index = m_this->_cur_bit_index >> 3;
    int bv_bit_index = m_this->_cur_bit_index & 0x07;

    for (i = offset; i < bit_size + offset; ++i) {
        byte_offset = i >> 3;
        bit_offset = i & 0x07;

        bv_byte_index = m_this->_cur_bit_index >> 3;
        bv_bit_index = m_this->_cur_bit_index & 0x07;

        if ((src_ptr[byte_offset] >> bit_offset) & 0x01) {
            m_this->_byte_pointer[bv_byte_index] |= (0x01 << bv_bit_index);
        }
        else {
            m_this->_byte_pointer[bv_byte_index] &= ~(0x01 << bv_bit_index);
        }

        ++m_this->_cur_bit_index;
    }

}

void set_bits_reverse(pHyBitVector m_this, const void* src_pointer, int offset, int bit_size) {
    const uint8_t * src_ptr = (uint8_t * ) src_pointer;

    int i = 0;
    int byte_offset = 0;
    int bit_offset = offset;

    int bv_byte_index = m_this->_cur_bit_index >> 3;
    int bv_bit_index = m_this->_cur_bit_index & 0x07;

    for (i = offset + bit_size - 1; i >= offset; --i) {
        byte_offset = i >> 3;
        bit_offset = i & 0x07;

        bv_byte_index = m_this->_cur_bit_index >> 3;
        bv_bit_index = m_this->_cur_bit_index & 0x07;

        if ((src_ptr[byte_offset] >> bit_offset) & 0x01) {
            m_this->_byte_pointer[bv_byte_index] |= (0x01 << bv_bit_index);
        }
        else {
            m_this->_byte_pointer[bv_byte_index] &= ~(0x01 << bv_bit_index);
        }

        ++m_this->_cur_bit_index;
    }

}

