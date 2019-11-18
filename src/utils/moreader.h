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

#ifndef MOREADER_H
#define MOREADER_H

#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>

class MoReader {
public:
    MoReader();
    ~MoReader();
    int Load(const char *filename);
    char* getText(char* text);
    
private:
    void abort();
    int readFromOffset(const int offset);
    unsigned long pjwHash(const char* str_param);
    int getTargetIndex(const char* s);
    bool labelMatches(const char *s, int index);
    char* getSourceString(int index);
    char* getTargetString(int index);
    int LoadFromFile(const char *filename, void **modata);
    
    // Data
    void *mo_data;
    int reversed;
    int num_strings;
    int original_table_offset;
    int translated_table_offset;
    int hash_num_entries;
    int hash_offset;
};


#endif /* MOREADER_H */