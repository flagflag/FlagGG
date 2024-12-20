/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <string.h>

#include "YGNodeList.h"

extern YGMalloc gYGMalloc;
extern YGRealloc gYGRealloc;
extern YGFree gYGFree;

struct YGNodeList {
  uint32_t capacity;
  uint32_t count;
  YGNodeRef *items;
};

YGNodeListRef YGNodeListNew(const uint32_t initialCapacity) {
  // BEGIN_UNITY @jonathanma CompileForVS2010
  const YGNodeListRef list = (YGNodeListRef)gYGMalloc(sizeof(struct YGNodeList));
  // END_UNITY
  YGAssert(list != NULL, "Could not allocate memory for list");

  list->capacity = initialCapacity;
  list->count = 0;
  // BEGIN_UNITY @jonathanma CompileForVS2010
  list->items = (YGNodeRef*)gYGMalloc(sizeof(YGNodeRef) * list->capacity);
  // END_UNITY
  YGAssert(list->items != NULL, "Could not allocate memory for items");

  return list;
}

void YGNodeListFree(const YGNodeListRef list) {
  if (list) {
    gYGFree(list->items);
    gYGFree(list);
  }
}

uint32_t YGNodeListCount(const YGNodeListRef list) {
  if (list) {
    return list->count;
  }
  return 0;
}

void YGNodeListAdd(YGNodeListRef *listp, const YGNodeRef node) {
  if (!*listp) {
    *listp = YGNodeListNew(4);
  }
  YGNodeListInsert(listp, node, (*listp)->count);
}

void YGNodeListInsert(YGNodeListRef *listp, const YGNodeRef node, const uint32_t index) {
  if (!*listp) {
    *listp = YGNodeListNew(4);
  }
  YGNodeListRef list = *listp;

  if (list->count == list->capacity) {
    list->capacity *= 2;
    // BEGIN_UNITY @jonathanma CompileForVS2010
    list->items = (YGNodeRef*)gYGRealloc(list->items, sizeof(YGNodeRef) * list->capacity);
    // END_UNITY
    YGAssert(list->items != NULL, "Could not extend allocation for items");
  }

  for (uint32_t i = list->count; i > index; i--) {
    list->items[i] = list->items[i - 1];
  }

  list->count++;
  list->items[index] = node;
}

void YGNodeListReplace(YGNodeListRef list, const uint32_t index, const YGNodeRef newNode) {
  list->items[index] = newNode;
}

void YGNodeListRemoveAll(const YGNodeListRef list) {
  for (uint32_t i = 0; i < list->count; i++) {
    list->items[i] = NULL;
  }
  list->count = 0;
}

YGNodeRef YGNodeListRemove(const YGNodeListRef list, const uint32_t index) {
  const YGNodeRef removed = list->items[index];
  list->items[index] = NULL;

  for (uint32_t i = index; i < list->count - 1; i++) {
    list->items[i] = list->items[i + 1];
    list->items[i + 1] = NULL;
  }

  list->count--;
  return removed;
}

YGNodeRef YGNodeListDelete(const YGNodeListRef list, const YGNodeRef node) {
  for (uint32_t i = 0; i < list->count; i++) {
    if (list->items[i] == node) {
      return YGNodeListRemove(list, i);
    }
  }

  return NULL;
}

YGNodeRef YGNodeListGet(const YGNodeListRef list, const uint32_t index) {
  if (YGNodeListCount(list) > 0) {
    return list->items[index];
  }

  return NULL;
}

YGNodeListRef YGNodeListClone(const YGNodeListRef oldList) {
  if (!oldList) {
    return NULL;
  }
  const uint32_t count = oldList->count;
  if (count == 0) {
    return NULL;
  }
  const YGNodeListRef newList = YGNodeListNew(count);
  memcpy(newList->items, oldList->items, sizeof(YGNodeRef) * count);
  newList->count = count;
  return newList;
}
