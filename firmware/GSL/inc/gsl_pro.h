#pragma once

// The GSL_PRO class includes functions to help profile code as it is being run
// with minimal overhead.

#include "gsl_includes.h"

// maximum number of things we can profile at once
const uint16_t gsl_pro_item_capacity = 200;

// separator between levels
const char gsl_pro_separator[] = " - ";

// number of items in current profile list
uint16_t gsl_pro_item_length = 0;

// a line item of profiling
struct GSL_PRO_Item {
  // text description of the item
  const char * text;
  // number of hits for this item
  uint32_t hits;
  // total number of ticks for this item
  uint64_t ticks;
  // initializer
  GSL_PRO_Item(const char * text = nullptr) :
    text(text),
    hits(0),
    ticks(0) {
  }
};

// ticks at last item
uint32_t gsl_pro_start_ticks = 0;

// tick at last checkpoint
uint32_t gsl_pro_checkpoint_ticks = 0;

// true if checkpoint is set
bool gsl_pro_checkpoint_set = false;

// list of items
GSL_PRO_Item gsl_pro_item[gsl_pro_item_capacity];

// item for overhead
GSL_PRO_Item & gsl_pro_overhead = gsl_pro_item[0];

// reset the timer
void GSL_PRO_ResetTimer(void) {
  gsl_pro_start_ticks = GSL_DEL_Ticks();
  gsl_pro_checkpoint_set = false;
}

// clear all items and reset the timer
void GSL_PRO_Reset(void) {
  gsl_pro_item_length = 1;
  gsl_pro_item[0] = "Profiling overhead";
  GSL_PRO_ResetTimer();
}

// convert a time in seconds to a human-readable format
// 0s
// 0.01ns
// 13.1ns
//  131ns
// 1.31us
// 13.1us
//  131us
// ns, us, ms, s
const char * GSL_PRO_LogTime(float seconds) {
  static char text[21];

  if (seconds <= 0.0f) {
    return "0 s";
  }
  // convert to digits
  uint64_t digits = seconds * 1e11f;
  // SSS.MMMUUUNNNPP
  for (uint16_t i = 0; i < 14; ++i) {
    text[13 - i] = '0' + digits % 10;
    digits /= 10u;
  }
  if (digits) {
    strcpy(text, GSL_OUT_Integer(seconds + 0.5f));
    strcpy(&text[strlen(text)], " s");
    return text;
  }
  const char * prefix[4] = {"s", "ms", "us", "ns"};
  uint16_t first_nonzero = 0;
  while (first_nonzero < 11 && text[first_nonzero] == '0') {
    ++first_nonzero;
  }
  if (first_nonzero % 3 == 0) {
    text[0] = text[first_nonzero];
    text[1] = text[first_nonzero + 1];
    text[2] = text[first_nonzero + 2];
    text[3] = 0;
    strcpy(&text[strlen(text)], " ");
    strcpy(&text[strlen(text)], prefix[first_nonzero / 3]);
  } else if (first_nonzero % 3 == 1) {
    text[0] = text[first_nonzero];
    text[1] = text[first_nonzero + 1];
    text[2] = '.';
    text[3] = text[first_nonzero + 2];
    text[4] = 0;
    strcpy(&text[strlen(text)], " ");
    strcpy(&text[strlen(text)], prefix[first_nonzero / 3]);
  } else if (first_nonzero % 3 == 2) {
    text[0] = text[first_nonzero];
    text[1] = '.';
    text[2] = text[first_nonzero + 1];
    text[3] = text[first_nonzero + 2];
    text[4] = 0;
    strcpy(&text[strlen(text)], " ");
    strcpy(&text[strlen(text)], prefix[first_nonzero / 3]);
  }
  return text;
}


void GSL_PRO_Test(void) {
  float time1 = 1000.1f;
  float time2 = 999.9f;
  for (uint16_t i = 0; i < 17; ++i) {
    LOG("\n", GSL_PRO_LogTime(time1), ", ");
    LOG(GSL_PRO_LogTime(time2));
    time1 /= 10.0f;
    time2 /= 10.0f;
  }
  HALT("");
}

// return a pointer to the given item
GSL_PRO_Item & GSL_PRO_FindItem(const char * text) {
  // look for this within existing items
  for (uint16_t i = 0; i < gsl_pro_item_length; ++i) {
    if (strcmp(text, gsl_pro_item[i].text) == 0) {
      return gsl_pro_item[i];
    }
  }
  // create a new line item
  ASSERT_LT(gsl_pro_item_length, gsl_pro_item_capacity);
  gsl_pro_item[gsl_pro_item_length].text = text;
  gsl_pro_item[gsl_pro_item_length].hits = 0;
  gsl_pro_item[gsl_pro_item_length].ticks = 0;
  ++gsl_pro_item_length;
  return gsl_pro_item[gsl_pro_item_length - 1];
}

// add ticks to an item
void GSL_PRO_AddToItem(const char * text, uint32_t ticks_to_add) {
  // find the item and add ticks
  GSL_PRO_Item & item = GSL_PRO_FindItem(text);
  ++item.hits;
  item.ticks += ticks_to_add;
}

// add ticks to an item
void GSL_PRO_Add(const char * text) {
  // ensure the text is in the FLASH memory
  ASSERT(IS_FLASH_ADDRESS((uint32_t) text));
  // mark start of overhead ticks
  const uint32_t start_overhead = GSL_DEL_Ticks();
  // mark ticks to add
  const uint32_t ticks_to_add = start_overhead - gsl_pro_start_ticks;
  // add ticks to item
  GSL_PRO_AddToItem(text, ticks_to_add);
  // reset checkpoint
  gsl_pro_checkpoint_set = false;
  // add overhead ticks
  const uint32_t overhead_ticks =
      GSL_DEL_Ticks() - start_overhead;
  gsl_pro_overhead.ticks += overhead_ticks;
  // advance timer
  gsl_pro_start_ticks += overhead_ticks + ticks_to_add;
}

// mark a checkpoint
void GSL_PRO_MarkCheckpoint(void) {
  gsl_pro_checkpoint_ticks = GSL_DEL_Ticks();
  gsl_pro_checkpoint_set = true;
}

// add time since last checkpoint to a given item
void GSL_PRO_AddFromCheckpoint(const char * text) {
  ASSERT(gsl_pro_checkpoint_set);
  // mark start of overhead ticks
  const uint32_t start_overhead = GSL_DEL_Ticks();
  // mark ticks to add
  const uint32_t ticks_to_add = start_overhead - gsl_pro_checkpoint_ticks;
  // add ticks to item
  GSL_PRO_AddToItem(text, ticks_to_add);
  // reset checkpoint
  gsl_pro_checkpoint_set = false;
  // add overhead ticks
  const uint32_t overhead_ticks =
      GSL_DEL_Ticks() - start_overhead;
  gsl_pro_overhead.ticks += overhead_ticks;
  // advance timer
  gsl_pro_start_ticks += overhead_ticks + ticks_to_add;
}

// output the profile tree to the debug log
void GSL_PRO_Output_OLD(void) {
  // get the system clock
  uint32_t clock = HAL_RCC_GetSysClockFreq();
  // count the total hits
  uint32_t total_hits = 0;
  // count the total ticks
  uint64_t total_ticks = 0;
  // count max hits
  uint32_t max_hits = 0;
  for (uint16_t i = 0; i < gsl_pro_item_length; ++i) {
    const auto & item = gsl_pro_item[i];
    total_hits += item.hits;
    total_ticks += item.ticks;
    if (item.hits > max_hits) {
      max_hits = item.hits;
    }
  }
  // maximum ticks for a single item
  uint64_t max_ticks = 0;
  // maximum length for each item
  uint16_t max_width[3] = {14, 4, 4};
  // get max length of each item
  for (uint16_t i = 0; i <= gsl_pro_item_length; ++i) {
    const auto & item =
        (i == gsl_pro_item_length) ? gsl_pro_overhead : gsl_pro_item[i];
    if (item.ticks > max_ticks) {
      max_ticks = item.ticks;
    }
    uint16_t length = strlen(item.text);
    if (length > max_width[0]) {
      max_width[0] = length;
    }
    length = strlen(GSL_OUT_Integer(item.hits));
    if (length > max_width[1]) {
      max_width[1] = length;
    }
    {
      const char * text = GSL_PRO_LogTime(item.ticks * 1.0f / clock);
      length = strlen(text);
      if (length > max_width[2]) {
        max_width[2] = length;
      }
    }
  }
  // output items
  LOG("\n\n");
  GSL_OUT_LogPaddedText("Profiling item", max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText("Hits", max_width[1], true);
  LOG("   ");
  GSL_OUT_LogPaddedText("Time", max_width[2], true);
  for (uint16_t i = 0; i <= gsl_pro_item_length; ++i) {
    const auto & item =
        (i == gsl_pro_item_length) ? gsl_pro_overhead : gsl_pro_item[i];
    LOG("\n");
    GSL_OUT_LogPaddedText(item.text, max_width[0], false);
    LOG("   ");
    {
      const char * text = GSL_OUT_Integer(item.hits);
      GSL_OUT_LogPaddedText(text, max_width[1], true);
    }
    LOG("   ");
    {
      const char * text = GSL_PRO_LogTime(item.ticks * 1.0f / clock);
      GSL_OUT_LogPaddedText(text, max_width[2], true);
    }
    // count number of Xs to add
    uint16_t x_count = 10.0f * item.ticks / max_ticks + 0.5f;
    if (x_count > 0) {
      LOG("   ");
      while (x_count > 0) {
        LOG("X");
        --x_count;
      }
    }
  }
  LOG("\n");
  GSL_OUT_LogPaddedText("---", max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText("---", max_width[1], true);
  LOG("   ");
  GSL_OUT_LogPaddedText("---", max_width[2], true);
  LOG("\n");
  GSL_OUT_LogPaddedText("Total", max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText(GSL_OUT_Integer(total_hits), max_width[1], true);
  LOG("   ");
  {
    const char * text = GSL_PRO_LogTime(total_ticks * 1.0f / clock);
    GSL_OUT_LogPaddedText(text, max_width[2], true);
  }
  LOG("\n\n");
}

// return -1 if one < two, 1 if one < two, and 0 if one = two
int GSL_PRO_CompareText(const char * one, const char * two, bool max_depth) {
  uint16_t one_length = strlen(one);
  uint16_t two_length = strlen(two);
  if (!max_depth) {
    const char * end = strstr(one, gsl_pro_separator);
    if (end) {
      one_length = end - one;
    }
    end = strstr(two, gsl_pro_separator);
    if (end) {
      two_length = end - two;
    }
  }
  uint16_t min_length = (one_length < two_length) ? one_length : two_length;
  int result = memcmp(one, two, min_length);
  if (result != 0) {
    return result;
  }
  if (one_length < two_length) {
    return -1;
  }
  if (two_length < one_length) {
    return 1;
  }
  return 0;
}

// return a pointer to the nth level if it exists, else return nullptr
const char * GSL_PRO_GetTextAtLevel(const char * text, uint16_t depth) {
  while (depth > 0) {
    text = strstr(text, gsl_pro_separator);
    if (text == nullptr) {
      return nullptr;
    }
    text += strlen(gsl_pro_separator);
    --depth;
  }
  return text;
}

// output the profile tree to the debug log
void GSL_PRO_Output(void) {

  // indent for each level
  const char indent[] = "  ";

  // break between columns
  const char column_break[] = "   ";

  // separator
  const char total_separator[] = "---";

  // maximum depth to print
  const uint16_t max_depth = 5;
  // pointers to each depth
  const char * depth_text[max_depth];

  // get the system clock
  uint32_t clock = HAL_RCC_GetSysClockFreq();
  // count the total hits
  uint32_t total_hits = 0;
  // count the total ticks
  uint64_t total_ticks = 0;
  // count max hits
  uint32_t max_hits = 0;
  for (uint16_t i = 0; i < gsl_pro_item_length; ++i) {
    const auto & item = gsl_pro_item[i];
    total_hits += item.hits;
    total_ticks += item.ticks;
    if (item.hits > max_hits) {
      max_hits = item.hits;
    }
  }
  // maximum ticks for a single item
  uint64_t max_ticks = 0;
  // header for each column
  const uint16_t header_count = 3;
  const char * header[header_count] = {"Profiling item", "Hits", "Time"};
  // maximum length for each item
  uint16_t max_width[header_count];
  for (uint16_t i = 0; i < header_count; ++i) {
    max_width[i] = strlen(header[i]);
  }
  {
    const uint16_t width = strlen(GSL_OUT_Integer(max_hits));
    if (width > max_width[1]) {
      max_width[1] = width;
    }
  }
  {
    const uint16_t width = strlen(GSL_OUT_Integer(max_hits));
    if (width > max_width[1]) {
      max_width[1] = width;
    }
  }
  // get max length of each item
  for (uint16_t i = 0; i < gsl_pro_item_length; ++i) {
    const auto & item = gsl_pro_item[i];
    if (item.ticks > max_ticks) {
      max_ticks = item.ticks;
    }
    {
      for (uint16_t d = 0; d < max_depth; ++d) {
        const char * next_item = nullptr;
        if (d < max_depth - 1) {
          next_item = GSL_PRO_GetTextAtLevel(item.text, d + 1);
        }
        const char * this_item = GSL_PRO_GetTextAtLevel(item.text, d);
        if (this_item == nullptr) {
          continue;
        }
        uint16_t length = d * strlen(indent);
        length += strlen(this_item);
        if (next_item != nullptr) {
          length -= strlen(next_item);
        }
        if (length > max_width[0]) {
          max_width[0] = length;
        }
      }
    }
    {
      const uint16_t length = strlen(GSL_OUT_Integer(item.hits));
      if (length > max_width[1]) {
        max_width[1] = length;
      }
    }
    {
      const char * text = GSL_PRO_LogTime(item.ticks * 1.0f / clock);
      const uint16_t length = strlen(text);
      if (length > max_width[2]) {
        max_width[2] = length;
      }
    }
  }
  // get max length of totals
  {
    uint16_t length = strlen("Total");
    if (length > max_width[0]) {
      max_width[0] = length;
    }
    length = strlen(GSL_OUT_Integer(total_hits));
    if (length > max_width[1]) {
      max_width[1] = length;
    }
    {
      const char * text = GSL_PRO_LogTime(total_ticks * 1.0f / clock);
      length = strlen(text);
      if (length > max_width[2]) {
        max_width[2] = length;
      }
    }
  }

  // output items
  LOG("\n\n");
  GSL_OUT_LogPaddedText(header[0], max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText(header[1], max_width[1], true);
  LOG("   ");
  GSL_OUT_LogPaddedText(header[2], max_width[2], true);

  // print out separator line
  LOG("\n");
  GSL_OUT_LogPaddedText(total_separator, max_width[0], false);
  LOG(column_break);
  GSL_OUT_LogPaddedText(total_separator, max_width[1], true);
  LOG(column_break);
  GSL_OUT_LogPaddedText(total_separator, max_width[2], true);

  // current depth
  uint16_t depth = 0;
  depth_text[0] = nullptr;
  // find maximum column width for each item
  bool done = false;
  while (true) {
    // hold the index of the exact match for this entry (once found)
    uint16_t matching_index = gsl_pro_item_length;
    // search for a sublevel
    if (depth_text[0] != nullptr && depth < max_depth - 1) {
      ++depth;
      depth_text[depth] = nullptr;
    }
    // search for next item on this level
    while (true) {
      const char * next_text = nullptr;
      for (uint16_t i = 0; i < gsl_pro_item_length; ++i) {
        // ensure all parent levels are the same
        uint16_t d = 0;
        for (; d < depth; ++d) {
          const char * text = GSL_PRO_GetTextAtLevel(gsl_pro_item[i].text, d);
          if (GSL_PRO_CompareText(text, depth_text[d], false) != 0) {
            break;
          }
        }
        if (d != depth) {
          continue;
        }
        // look at this level to see if it's newer
        const char * text = GSL_PRO_GetTextAtLevel(gsl_pro_item[i].text, d);
        // test if this entry is after the current entry
        if (depth_text[d] == nullptr ||
            GSL_PRO_CompareText(depth_text[d], text, d == max_depth - 1) < 0) {
          if (next_text == nullptr ||
              GSL_PRO_CompareText(text, next_text, d == max_depth - 1) <= 0) {
            // actual text has changed
            if (GSL_PRO_CompareText(text, next_text, d == max_depth - 1) != 0) {
              matching_index = gsl_pro_item_length;
            }
            next_text = text;
            // if next sublevel is empty, set this as the matching item
            if (d == max_depth - 1 ||
                GSL_PRO_GetTextAtLevel(gsl_pro_item[i].text, d + 1) == nullptr) {
              matching_index = i;
            }
          }
        }
      }
      // if we found an item, print it out
      if (next_text != nullptr) {
        depth_text[depth] = next_text;
        break;
      }
      // else decrease the depth if possible
      if (depth == 0) {
        done = true;
        break;
      } else {
        --depth;
      }
    }
    if (done) {
      break;
    }
    // print out this item
    LOG("\n");
    uint16_t spaces_left = max_width[0];
    for (uint16_t i = 0; i < depth; ++i) {
      LOG(indent);
      spaces_left -= strlen(indent);
    }
    const char * end = strstr(depth_text[depth], gsl_pro_separator);
    if (end != nullptr && depth < max_depth - 1) {
      uint16_t length = end - depth_text[depth];
      GSL_DELOG_AddBytes((const uint8_t *) depth_text[depth], length);
      spaces_left -= length;
    } else {
      LOG(depth_text[depth]);
      spaces_left -= strlen(depth_text[depth]);
    }
    if (false && matching_index == gsl_pro_item_length) {
      continue;
    }
    while (spaces_left) {
      LOG(" ");
      --spaces_left;
    }
    // search for this exact item
    // print it out, if it exists
    if (matching_index != gsl_pro_item_length) {
      const auto & item = gsl_pro_item[matching_index];
      LOG(column_break);
      {
        const char * text = GSL_OUT_Integer(item.hits);
        GSL_OUT_LogPaddedText(text, max_width[1], true);
      }
      LOG(column_break);
      {
        const char * text = GSL_PRO_LogTime(item.ticks * 1.0f / clock);
        GSL_OUT_LogPaddedText(text, max_width[2], true);
      }
      // count number of Xs to add
      uint16_t x_count = 10.0f * item.ticks / max_ticks + 0.5f;
      if (x_count > 0) {
        LOG(column_break);
        while (x_count > 0) {
          LOG("X");
          --x_count;
        }
      }
    } else {
      LOG(column_break);
      GSL_OUT_LogPaddedText("0", max_width[1], true);
      LOG(column_break);
      GSL_OUT_LogPaddedText("0 s", max_width[2], true);
    }
  }

  // print out separator line
  LOG("\n");
  GSL_OUT_LogPaddedText(total_separator, max_width[0], false);
  LOG(column_break);
  GSL_OUT_LogPaddedText(total_separator, max_width[1], true);
  LOG(column_break);
  GSL_OUT_LogPaddedText(total_separator, max_width[2], true);

  // print out totals
  LOG("\n");
  GSL_OUT_LogPaddedText("Total", max_width[0], false);
  LOG(column_break);
  GSL_OUT_LogPaddedText(GSL_OUT_Integer(total_hits), max_width[1], true);
  LOG(column_break);
  {
    const char * text = GSL_PRO_LogTime(total_ticks * 1.0f / clock);
    GSL_OUT_LogPaddedText(text, max_width[2], true);
  }
  LOG("\n\n");

  /*

  // if no items are present, just exit
  ASSERT_GT(gsl_pro_item_length, 0U);

  // find the lexigraphically first item
  const char * first = gsl_pro_item[0].text;
  for (uint16_t i = 1; i < gsl_pro_item_length; ++i) {
    if (strcmp(first, gsl_pro_item[i].text) > 0) {
      first = gsl_pro_item[i].text;
    }
  }

  // set up pointers


  // get the system clock
  uint32_t clock = HAL_RCC_GetSysClockFreq();
  // count the total hits
  uint32_t total_hits = 0;
  // count the total ticks
  uint64_t total_ticks = 0;
  // count max hits
  uint32_t max_hits = 0;
  for (uint16_t i = 0; i < gsl_pro_item_length; ++i) {
    const auto & item = gsl_pro_item[i];
    total_hits += item.hits;
    total_ticks += item.ticks;
    if (item.hits > max_hits) {
      max_hits = item.hits;
    }
  }
  // maximum ticks for a single item
  uint64_t max_ticks = 0;
  // maximum length for each item
  uint16_t max_width[3] = {14, 4, 4};
  // get max length of each item
  for (uint16_t i = 0; i <= gsl_pro_item_length; ++i) {
    const auto & item =
        (i == gsl_pro_item_length) ? gsl_pro_overhead : gsl_pro_item[i];
    if (item.ticks > max_ticks) {
      max_ticks = item.ticks;
    }
    uint16_t length = strlen(item.text);
    if (length > max_width[0]) {
      max_width[0] = length;
    }
    length = strlen(GSL_OUT_Integer(item.hits));
    if (length > max_width[1]) {
      max_width[1] = length;
    }
    {
      const char * text = GSL_PRO_LogTime(item.ticks * 1.0f / clock);
      length = strlen(text);
      if (length > max_width[2]) {
        max_width[2] = length;
      }
    }
  }
  // output items
  LOG("\n\n");
  GSL_OUT_LogPaddedText("Profiling item", max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText("Hits", max_width[1], true);
  LOG("   ");
  GSL_OUT_LogPaddedText("Time", max_width[2], true);
  for (uint16_t i = 0; i <= gsl_pro_item_length; ++i) {
    const auto & item =
        (i == gsl_pro_item_length) ? gsl_pro_overhead : gsl_pro_item[i];
    LOG("\n");
    GSL_OUT_LogPaddedText(item.text, max_width[0], false);
    LOG("   ");
    {
      const char * text = GSL_OUT_Integer(item.hits);
      GSL_OUT_LogPaddedText(text, max_width[1], true);
    }
    LOG("   ");
    {
      const char * text = GSL_PRO_LogTime(item.ticks * 1.0f / clock);
      GSL_OUT_LogPaddedText(text, max_width[2], true);
    }
    // count number of Xs to add
    uint16_t x_count = 10.0f * item.ticks / max_ticks + 0.5f;
    if (x_count > 0) {
      LOG("   ");
      while (x_count > 0) {
        LOG("X");
        --x_count;
      }
    }
  }
  LOG("\n");
  GSL_OUT_LogPaddedText("---", max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText("---", max_width[1], true);
  LOG("   ");
  GSL_OUT_LogPaddedText("---", max_width[2], true);
  LOG("\n");
  GSL_OUT_LogPaddedText("Total", max_width[0], false);
  LOG("   ");
  GSL_OUT_LogPaddedText(GSL_OUT_Integer(total_hits), max_width[1], true);
  LOG("   ");
  {
    const char * text = GSL_PRO_LogTime(total_ticks * 1.0f / clock);
    GSL_OUT_LogPaddedText(text, max_width[2], true);
  }
  LOG("\n\n");
  */
}
