#pragma once

#include "gsl_includes.h"

// touchscreen object
GSL_TOUCH_STMPE610 * gsl_tsm_touch = nullptr;

// lcd object
GSL_LCD_HX8357D * gsl_tsm_lcd = nullptr;

// This implements a LCD touchscreen based menu

// menu item types
enum GSL_TSM_ItemTypeEnum : uint8_t {
  // unused
  kMenuTypeUnused = 0,
  // disabled item (grayed out, unresponsive to button press)
  kMenuTypeInactive,
  // call a function when pressed
  kMenuTypeCallFunction,
  // don't do anything special
  // (we can still active a different menu via new_menu if defined)
  kMenuTypeNone,
  // show more options (parameter is pointer to this menu)
  kMenuTypeMoreOptions,
  // go up one menu level (parameter is pointer to the parent menu)
  //kMenuTypeGotoParent,
  // special type used to denote the end of the list
  kMenuTypeEnd,
};

// forwared declaration
//struct GSL_TSM_MenuItemStruct;

// holds a collection of menu items
//typedef struct GSL_TSM_MenuItemStruct * GSL_TSM_MenuStruct;

// menu item struct
struct GSL_TSM_MenuItemStruct {
  // menu item text
  const char * text;
  // menu item type
  GSL_TSM_ItemTypeEnum item_type;
  // parameter for item_type, if applicable, else null
  uint32_t parameter;
  // if not null, switch to the given menu after the button is pressed
  const GSL_TSM_MenuItemStruct * new_menu;
};

// button structure
struct GSL_TSM_ButtonStruct {
  // extent
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
  // text to display
  //const char * text;
  // true if active
  bool active;
  // pointer to this menu item, or nullptr
  const GSL_TSM_MenuItemStruct * item;
  // pointer to the menu this is a part of, or nullptr
  const GSL_TSM_MenuItemStruct * menu;
};

// maximum buttons present at once
const uint16_t gsl_tsm_button_capacity = 6;

// number of buttons in use
uint16_t gsl_tsm_button_count = 0;

// button array
GSL_TSM_ButtonStruct gsl_tsm_button[gsl_tsm_button_capacity];

extern const GSL_TSM_MenuItemStruct kct_menu_main[];

// View menu
const GSL_TSM_MenuItemStruct kct_menu_view[] = {
    {"Adapters", kMenuTypeNone, 0, nullptr},
    {"Connectivity", kMenuTypeNone, 0, nullptr},
    {"Item 3", kMenuTypeNone, 0, nullptr},
    {"Item 4", kMenuTypeNone, 0, nullptr},
    {"", kMenuTypeEnd, 0, kct_menu_main}};

// Help menu
const GSL_TSM_MenuItemStruct kct_menu_help[] = {
    {"About", kMenuTypeNone, 0, kct_menu_main},
    {"", kMenuTypeEnd, 0, kct_menu_main}};

// Main menu
const GSL_TSM_MenuItemStruct kct_menu_main[] = {
    {"File", kMenuTypeInactive, 0, nullptr},
    {"View", kMenuTypeNone, 0, kct_menu_view},
    {"Help", kMenuTypeNone, 0, kct_menu_help},
    {"", kMenuTypeEnd, 0, nullptr}};

// menu structure
/*{0, "File"},
{1, "Open", FileOpenFunction},
{0, "Help"},
{1, "About", KCT_AboutScreen},*/

// erase the given button index
void GSL_TSM_EraseButton(uint16_t index, bool erase_graphic = true) {
  // bound check
  ASSERT(index < gsl_tsm_button_count);
  // alias
  GSL_TSM_ButtonStruct & button = gsl_tsm_button[index];
  // erase the graphic
  if (erase_graphic) {
    gsl_tsm_lcd->Fill(
        button.x1,
        button.y1,
        button.x2,
        button.y2,
        gsl_tsm_lcd->color_.back);
  }
  // erase it from the index
  --gsl_tsm_button_count;
  // overwrite this button with the last in the list
  if (index < gsl_tsm_button_count) {
    gsl_tsm_button[index] = gsl_tsm_button[gsl_tsm_button_count];
  }
}

// erase and delete all buttons
void GSL_TSM_EraseButtons(bool erase_graphic = true) {
  while (gsl_tsm_button_count) {
    GSL_TSM_EraseButton(gsl_tsm_button_count - 1, erase_graphic);
  }
}

// erase and delete all buttons associated with the given menu
void GSL_TSM_EraseMenu(const GSL_TSM_MenuItemStruct * menu) {
  for (uint16_t i = gsl_tsm_button_count; i > 0; --i) {
    // alias this button
    GSL_TSM_ButtonStruct & button = gsl_tsm_button[i - 1];
    if (button.menu == menu) {
      GSL_TSM_EraseButton(i);
    }
  }
}

// add a button
void GSL_TSM_AddButton(
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    const GSL_TSM_MenuItemStruct * item,
    const GSL_TSM_MenuItemStruct * menu = nullptr) {
  //ASSERT(item);
  // check for memory space
  if (gsl_tsm_button_count == gsl_tsm_button_capacity) {
    HALT("Not enough buttons");
  }
  // alias this button
  GSL_TSM_ButtonStruct & button = gsl_tsm_button[gsl_tsm_button_count];
  ++gsl_tsm_button_count;
  button.x1 = x;
  button.y1 = y;
  button.x2 = x + width - 1;
  button.y2 = y + height - 1;
  button.item = item;
  button.active = (item && item->item_type != kMenuTypeInactive);
  button.menu = menu;
}

// draw a button
void GSL_TSM_DrawButton(GSL_TSM_ButtonStruct & button, bool touched = false) {
  const auto text_color = gsl_tsm_lcd->color_.text;
  if (!button.active) {
    gsl_tsm_lcd->color_.text = gsl_tsm_lcd->ToColor(0.25f, 0.25f, 0.0f);
  }
  const char * text = "";
  if (button.item) {
    text = button.item->text;
  }
  gsl_tsm_lcd->DrawButton(
      text,
      button.x1,
      button.y1,
      button.x2,
      button.y2,
      touched);
  if (!button.active) {
    gsl_tsm_lcd->color_.text = text_color;
  }
}

// show more menu options

// display the given menu
void GSL_TSM_Display(
    const GSL_TSM_MenuItemStruct * menu,
    uint16_t item_offset = 0) {

  // button to go up one level in menus
  static GSL_TSM_MenuItemStruct up_button =
      {"Up", kMenuTypeInactive, 0, nullptr};

  // button to display more options
  static GSL_TSM_MenuItemStruct right_button =
      {">>", kMenuTypeInactive, 0, nullptr};

  // 480 == 2 * arrowButtonWidth + buttonSpacing + (buttonWidth + buttonSpacing) * buttonCount
  // 480 == 2 * arrowButtonWidth + 4 * buttonSpacing + 3 * buttonWidth
  // 480 == 2 * 40 + 4 * 4 + 3 * buttonWidth

  // height of all buttons
  const uint16_t buttonHeight = 35;
  // width of normal buttons
  const uint16_t buttonWidth = 128;
  // width of next/previous buttons
  const uint16_t arrowButtonWidth = 40;
  // pixels between buttons
  const uint16_t buttonSpacing = 4;
  // number of normal buttons able to be displayed
  uint16_t normalButtonCount = (gsl_tsm_lcd->width_ - 2 * (arrowButtonWidth) - buttonSpacing) / (buttonWidth + buttonSpacing);

  // needed in GSL_TSM_ProcessButtonPress below
  ASSERT(normalButtonCount == 3);

  // maximum number of items allowed in a menu
  const uint16_t max_item_count = 50;

  // number of items in this menu
  uint16_t item_count = 0;
  while (menu[item_count].item_type != kMenuTypeEnd) {
    ++item_count;
    ASSERT(item_count <= max_item_count);
  }

  // put parent menu in up button
  up_button.new_menu = menu[item_count].new_menu;
  // deactivate it if we're at the top level
  if (up_button.new_menu) {
    up_button.item_type = kMenuTypeNone;
  } else {
    up_button.item_type = kMenuTypeInactive;
  }

  // put more options button
  if (item_count <= normalButtonCount) {
    right_button.item_type = kMenuTypeInactive;
  } else {
    right_button.item_type = kMenuTypeMoreOptions;
    right_button.new_menu = menu;
    if (item_offset + normalButtonCount >= item_count) {
      right_button.parameter = 0;
    } else {
      right_button.parameter = item_offset + normalButtonCount;
    }
  }

  // erase all previous buttons but leave the graphic there
  GSL_TSM_EraseButtons(false);

  uint16_t x = 0;
  uint16_t y = 320 - buttonHeight;

  // up button
  GSL_TSM_AddButton(x, y, arrowButtonWidth, buttonHeight, &up_button, menu);
  x += arrowButtonWidth;
  x += buttonSpacing;

  // normal buttons
  for (uint16_t i = 0; i < normalButtonCount; ++i) {
    if (i + item_offset < item_count) {
      GSL_TSM_AddButton(x, y, buttonWidth, buttonHeight, &menu[i + item_offset], nullptr);
    } else {
      GSL_TSM_AddButton(x, y, buttonWidth, buttonHeight, nullptr);
    }
    x += buttonWidth;
    x += buttonSpacing;
  }

  // right button
  GSL_TSM_AddButton(x, y, arrowButtonWidth, buttonHeight, &right_button, menu);

  // draw buttons
  for (uint16_t i = 0; i < gsl_tsm_button_count; ++i) {
    GSL_TSM_DrawButton(gsl_tsm_button[i]);
  }

}

// process a press of the given button
void GSL_TSM_ProcessButtonPress(uint16_t index = 0) {
  ASSERT(index < gsl_tsm_button_count);
  GSL_TSM_ButtonStruct & button = gsl_tsm_button[index];
  // if this displays more options, then do that
  switch (button.item->item_type) {
    case kMenuTypeCallFunction: {
      ASSERT(button.item->parameter);
      // call that function
      HALT("TODO");
      break;
    }
    case kMenuTypeNone: {
      // do nothing
      break;
    }
    case kMenuTypeMoreOptions: {
      uint16_t item_offset = (uint16_t) button.item->parameter;
      // display new options
      GSL_TSM_Display(button.item->new_menu, item_offset);
      // exit so we don't go to the new menu
      return;
      break;
    }
    default: {
      HALT("Unexpected case");
      break;
    }
  }
  // if this sends us to a new menu, then do it
  if (button.item->new_menu) {
    // display the new menu
    GSL_TSM_Display(button.item->new_menu);
  }
}

// touch processing routine
// (must call this every 50ms or so when menu is active)
void GSL_TSM_Process(void) {
  // detect new touch
  float x, y;
  if (gsl_tsm_touch->GetTouch(&x, &y)) {
    // see if it's inside of any buttons
    for (uint16_t i = 0; i < gsl_tsm_button_count; ++i) {
      GSL_TSM_ButtonStruct & button = gsl_tsm_button[i];
      // skip button if it's inactive
      if (!button.active) {
        continue;
      }
      // see if touch is within the bounds
      if (button.x1 <= x && button.x2 >= x &&
          button.y1 <= y && button.y2 >= y) {
        // draw the button depressed
        GSL_TSM_DrawButton(button, true);
        // wait for it to be released
        while (!gsl_tsm_touch->IsReleased()) {
        }
        // draw the released button
        GSL_TSM_DrawButton(button, false);
        // process the button press
        GSL_TSM_ProcessButtonPress(i);
        // exit, as button indices may have changed
        break;
      }
    }
  }
}
