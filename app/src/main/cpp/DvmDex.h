/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * The VM wraps some additional data structures around the DexFile.  These
 * are defined here.
 */
#ifndef DALVIK_DVMDEX_H_
#define DALVIK_DVMDEX_H_

#include <sys/types.h>
#include "jni.h"
#include "DexFile.h"
#include "DvmDex.h"

/* extern */
struct ClassObject;
struct HashTable;
struct InstField;
struct Method;
struct StringObject;


/*
 * Some additional VM data structures that are associated with the DEX file.
 */

struct MemMapping {
    void *addr;           /* start of data */
    size_t length;         /* length of data */

    void *baseAddr;       /* page-aligned base address */
    size_t baseLength;     /* length of mapping */
};

struct DvmDex {
    /* pointer to the DexFile we're associated with */
    DexFile *pDexFile;

    /* clone of pDexFile->pHeader (it's used frequently enough) */
    const DexHeader *pHeader;

    /* interned strings; parallel to "stringIds" */
    struct StringObject **pResStrings;

    /* resolved classes; parallel to "typeIds" */
    struct ClassObject **pResClasses;

    /* resolved methods; parallel to "methodIds" */
    struct Method **pResMethods;

    /* resolved instance fields; parallel to "fieldIds" */
    /* (this holds both InstField and StaticField) */
    struct Field **pResFields;

    /* interface method lookup cache */
    struct AtomicCache *pInterfaceCache;

    /* shared memory region with file contents */
    bool isMappedReadOnly;
    MemMapping memMap;

    jobject dex_object;

    /* lock ensuring mutual exclusion during updates */
    pthread_mutex_t modLock;
};


/*
 * Given a file descriptor for an open "optimized" DEX file, map it into
 * memory and parse the contents.
 *
 * On success, returns 0 and sets "*ppDvmDex" to a newly-allocated DvmDex.
 * On failure, returns a meaningful error code [currently just -1].
 */
int dvmDexFileOpenFromFd(int fd, DvmDex **ppDvmDex);

/*
 * Open a partial DEX file.  Only useful as part of the optimization process.
 */
int dvmDexFileOpenPartial(const void *addr, int len, DvmDex **ppDvmDex);

/*
 * Free a DvmDex structure, along with any associated structures.
 */
void dvmDexFileFree(DvmDex *pDvmDex);


/*
 * Change the 1- or 2-byte value at the specified address to a new value.  If
 * the location already has the new value, do nothing.
 *
 * This does not make any synchronization guarantees.  The caller must
 * ensure exclusivity vs. other callers.
 *
 * For the 2-byte call, the pointer should have 16-bit alignment.
 *
 * Returns "true" on success.
 */
bool dvmDexChangeDex1(DvmDex *pDvmDex, u1 *addr, u1 newVal);

bool dvmDexChangeDex2(DvmDex *pDvmDex, u2 *addr, u2 newVal);


#endif  // DALVIK_DVMDEX_H_
