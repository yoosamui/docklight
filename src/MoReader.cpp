//*****************************************************************
//
//  Copyright (c) 2007 Jonathan Blow (jon [at] number-none [dot] com)
//  Modify by Juan Gonzalez January 2017
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************


#include "MoReader.h"

#define HASHWORDBITS 32

MoReader::MoReader() {
    mo_data = NULL;
    num_strings = 0;
    original_table_offset = 0;
    translated_table_offset = 0;
    hash_num_entries = 0;
    hash_offset = 0;
}

int MoReader::Load(const char *filename) {

    void *data;
    int length = LoadFromFile(filename, &data);
    if (length < 0)
        return -1;

    if (length < 24) { // There has to be at least this much in the header...
        abort();
        return -1;
    }


    mo_data = data;
    unsigned long *long_ptr = (unsigned long *) data;
    const unsigned long TARGET_MAGIC = 0x950412DE;
    const unsigned long TARGET_MAGIC_REVERSED = 0xDE120495;
    unsigned long magic = long_ptr[0];

    if (magic == TARGET_MAGIC) {
        reversed = 0;
    } else if (magic == TARGET_MAGIC_REVERSED) {
        reversed = 1;
    } else {
        abort();
        return -1;
    }

    num_strings = readFromOffset(8);
    original_table_offset = readFromOffset(12);
    translated_table_offset = readFromOffset(16);
    hash_num_entries = readFromOffset(20);
    hash_offset = readFromOffset(24);

    // We expect a hash table to be there; if it's not, bail.
    if (hash_num_entries == 0) {
        abort();
        return -1;
    }


    return length;
}

int MoReader::LoadFromFile(const char *filename, void **modata) {
    assert(filename);

    FILE *f = fopen(filename, "rb");
    if (!f)
        return -1;

    // The function fileno() examines the argument stream and returns 
    // its integer descriptor.
    int descriptor = fileno(f);

    // The fstat() function shall obtain information about an open file associated 
    // with the file descriptor fildes, and shall write it to the area pointed to 
    // by buf. Upon successful completion, 0 shall be returned. Otherwise, -1 
    // shall be returned and errno set to indicate the error.
    struct stat file_stats;
    int result = fstat(descriptor, &file_stats);
    if (result == -1)
        return -1;

    int length = file_stats.st_size;
    unsigned char *data = new unsigned char[length];

    fseek(f, 0, SEEK_SET);
    int success = fread((void *) data, length, 1, f);
    if (success < 1) {
        delete [] data;
        return -1;
    }

    *modata = data;

    fclose(f);
    return length;
}

inline int MoReader::readFromOffset(const int offset) {

    unsigned long *ptr =
            (unsigned long *) (((char *) mo_data) + offset);

    if (reversed) {

        unsigned long c0 = (*ptr >> 0) & 0xff;
        unsigned long c1 = (*ptr >> 8) & 0xff;
        unsigned long c2 = (*ptr >> 16) & 0xff;
        unsigned long c3 = (*ptr >> 24) & 0xff;

        return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;

    } else {
        return *ptr;
    }
}

/**
 * PJW hash function is a non-cryptographic hash function 
 * created by Peter J. Weinberger of AT&T Bell Labs.
 * PJW hash algorithm involves shifting previous hash and adding current 
 * byte followed by moving high bits.
 * 
 * @param str_param
 * @return unsigned long
 */
inline unsigned long MoReader::pjwHash(const char* str_param) {

    unsigned long hval = 0;
    unsigned long g;
    const char *s = str_param;

    while (*s) {
        hval <<= 4;
        hval += (unsigned char) *s++;
        g = hval & ((unsigned long int) 0xf << (HASHWORDBITS - 4));
        if (g != 0) {
            hval ^= g >> (HASHWORDBITS - 8);
            hval ^= g;
        }
    }

    return hval;
}

inline char* MoReader::getSourceString(int index) {

    int addr_offset = original_table_offset + 8 * index + 4;
    int string_offset = readFromOffset(addr_offset);

    char *t = ((char *) mo_data) + string_offset;

    return t;
}

inline char* MoReader::getTargetString(int index) {

    int addr_offset = translated_table_offset + 8 * index + 4;
    int string_offset = readFromOffset(addr_offset);

    char *t = ((char *) mo_data) + string_offset;

    return t;
}

bool MoReader::labelMatches(const char *s, int index) {
    char *t = getSourceString(index);

    if (strcmp(s, t) == 0)
        return true;

    return false;
}

inline int MoReader::getTargetIndex(const char* s) {
    unsigned long V = pjwHash(s);
    int S = hash_num_entries;

    int hash_cursor = V % S;
    int orig_hash_cursor = hash_cursor;
    int increment = 1 + (V % (S - 2));

    while (1) {

        unsigned int index =
                readFromOffset(hash_offset + 4 * hash_cursor);

        if (index == 0)
            break;

        index--; // Because entries in the table are stored +1 so that 0 means empty.

        if (labelMatches(s, index))
            return index;

        hash_cursor += increment;
        hash_cursor %= S;

        if (hash_cursor == orig_hash_cursor)
            break;
    }

    return -1;
}

void MoReader::abort() {
    delete [] (char *) mo_data;
    mo_data = NULL;
    return;
}

MoReader::~MoReader() {
    if (mo_data) delete [] ((char *) mo_data);
}

char* MoReader::getText(char* text) {

    if (!mo_data)
        return text;

    int target_index = getTargetIndex(text);
    if (target_index == -1)
        return text;


    return getTargetString(target_index);

}

