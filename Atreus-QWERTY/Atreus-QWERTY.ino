/* -*- mode: c++ -*-
 * Atreus -- Chrysalis-enabled Sketch for the Keyboardio Atreus
 * Copyright (C) 2018-2022  Keyboard.io, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef BUILD_INFORMATION
#define KALEIDOSCOPE_FIRMWARE_VERSION "QWERTY + STENO"
#define BUILD_INFORMATION "locally built on " __DATE__ " at " __TIME__
#endif

#include "Kaleidoscope.h"
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-Escape-OneShot.h"
#include "Kaleidoscope-FirmwareVersion.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-DynamicMacros.h"
#include "Kaleidoscope-LayerNames.h"
#include "Kaleidoscope-Steno.h"


#define MO(n) ShiftToLayer(n)
#define TG(n) LockLayer(n)

enum {
  MACRO_QWERTY,
  MACRO_VERSION_INFO
};

#define Key_Exclamation LSHIFT(Key_1)
#define Key_At LSHIFT(Key_2)
#define Key_Hash LSHIFT(Key_3)
#define Key_Dollar LSHIFT(Key_4)
#define Key_Percent LSHIFT(Key_5)
#define Key_Caret LSHIFT(Key_6)
#define Key_And LSHIFT(Key_7)
#define Key_Star LSHIFT(Key_8)
#define Key_Plus LSHIFT(Key_Equals)
#define Key_Question LSHIFT(Key_Slash)
#define Key_VolUp Consumer_VolumeIncrement
#define Key_VolDown Consumer_VolumeDecrement

enum {
  QWERTY,
  SYM,
  CTRL,
  STENO
};

// clang-format off
KEYMAPS(
  [QWERTY] = KEYMAP_STACKED
  (
       Key_Q   ,Key_W   ,Key_E       ,Key_R         ,Key_T
      ,Key_A   ,Key_S   ,Key_D       ,Key_F         ,Key_G
      ,Key_Z   ,Key_X   ,Key_C       ,Key_V         ,Key_B, Key_Backtick
      ,Key_Esc ,Key_Tab ,Key_LeftGui ,Key_LeftShift ,Key_Backspace ,Key_LeftControl

                     ,Key_Y     ,Key_U      ,Key_I     ,Key_O      ,Key_P
                     ,Key_H     ,Key_J      ,Key_K     ,Key_L      ,Key_Semicolon
       ,Key_Backslash,Key_N     ,Key_M      ,Key_Comma ,Key_Period ,Key_Slash
       ,Key_LeftAlt  ,Key_Space ,MO(SYM)    ,Key_Minus ,Key_Quote  ,Key_Enter
  ),

  [SYM] = KEYMAP_STACKED
  (
       TG(CTRL)        ,MoveToLayer(STENO) ,Key_UpArrow   ,Key_Dollar           ,Key_Exclamation
      ,Key_LeftParen   ,Key_LeftArrow      ,Key_DownArrow ,Key_RightArrow       ,Key_RightParen
      ,Key_LeftBracket ,Key_RightBracket   ,Key_Hash      ,Key_LeftCurlyBracket ,Key_RightCurlyBracket ,Key_Question
      ,___             ,___                ,___           ,___                  ,___                   ,___

                ,Key_Caret  ,Key_7 ,Key_8      ,Key_9 ,Key_Backspace
                ,Key_Slash  ,Key_4 ,Key_5      ,Key_6 ,Key_Star
      ,Key_And  ,Key_Minus  ,Key_1 ,Key_2      ,Key_3 ,Key_Plus
      ,___      ,___        ,___   ,Key_Period ,Key_0 ,Key_Equals
   ),

  [CTRL] = KEYMAP_STACKED
  (
       Key_Insert            ,Key_Home      ,Key_UpArrow   ,Key_End         ,Key_PageUp
      ,Key_Delete            ,Key_LeftArrow ,Key_DownArrow ,Key_RightArrow  ,Key_PageDown
      ,XXX                   ,XXX           ,XXX ,XXX       ,XXX            ,XXX
      ,MoveToLayer(QWERTY)  ,___           ,___ ,___     ,___              ,___

            ,Key_VolUp    ,Key_F7  ,Key_F8          ,Key_F9         ,Key_F10
            ,Key_VolDown  ,Key_F4  ,Key_F5          ,Key_F6         ,Key_F11
      ,XXX  ,XXX          ,Key_F1  ,Key_F2          ,Key_F3         ,Key_F12
      ,___  ,___          ,XXX     ,Key_PrintScreen ,Key_ScrollLock ,Consumer_PlaySlashPause
   ),

  [STENO] = KEYMAP_STACKED
  (
       MoveToLayer(QWERTY) ,XXX   ,XXX    ,XXX    ,XXX
      ,S(S1)                ,S(TL) ,S(PL)  ,S(HL)  ,S(ST1)
      ,S(S2)                ,S(KL) ,S(WL)  ,S(RL)  ,S(ST2) ,S(ST3)
      ,MoveToLayer(QWERTY) ,XXX   ,XXX    ,XXX    ,S(A)   ,S(O)

              ,XXX   ,XXX   ,XXX    ,XXX   ,XXX
              ,S(FR) ,S(PR) ,S(LR)  ,S(TR) ,S(DR)
      ,S(ST4) ,S(RR) ,S(BR) ,S(GR)  ,S(SR) ,S(ZR)
      ,S(E)   ,S(U)  ,XXX   ,XXX    ,XXX   ,XXX
  )
)
// clang-format on

KALEIDOSCOPE_INIT_PLUGINS(
  // ----------------------------------------------------------------------
  // Chrysalis plugins

  // The EEPROMSettings & EEPROMKeymap plugins make it possible to have an
  // editable keymap in EEPROM.
  EEPROMSettings,
  EEPROMKeymap,

  // Focus allows bi-directional communication with the host, and is the
  // interface through which the keymap in EEPROM can be edited.
  Focus,

  // FocusSettingsCommand adds a few Focus commands, intended to aid in
  // changing some settings of the keyboard, such as the default layer (via the
  // `settings.defaultLayer` command)
  FocusSettingsCommand,

  // FocusEEPROMCommand adds a set of Focus commands, which are very helpful in
  // both debugging, and in backing up one's EEPROM contents.
  FocusEEPROMCommand,

  // The FirmwareVersion plugin lets Chrysalis query the version of the firmware
  // programmatically.
  FirmwareVersion,

  // The LayerNames plugin allows Chrysalis to display - and edit - custom layer
  // names, to be shown instead of the default indexes.
  LayerNames,

  // ----------------------------------------------------------------------
  // Keystroke-handling plugins

  // The Qukeys plugin enables the "Secondary action" functionality in
  // Chrysalis. Keys with secondary actions will have their primary action
  // performed when tapped, but the secondary action when held.
  Qukeys,

  // Enables the "Sticky" behavior for modifiers, and the "Layer shift when
  // held" functionality for layer keys.
  OneShot,
  OneShotConfig,
  EscapeOneShot,
  EscapeOneShotConfig,

  // The macros plugin adds support for macros
  Macros,

  // Enables dynamic, Chrysalis-editable macros.
  DynamicMacros,

  // The MouseKeys plugin lets you add keys to your keymap which move the mouse.
  MouseKeys,
  MouseKeysConfig,

  // The MagicCombo plugin lets you use key combinations to trigger custom
  // actions - a bit like Macros, but triggered by pressing multiple keys at the
  // same time.
  // MagicCombo,

  // Enables the GeminiPR Stenography protocol. Unused by default, but with the
  // plugin enabled, it becomes configurable - and then usable - via Chrysalis.
  GeminiPR);

const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    switch (macro_id) {
      case MACRO_VERSION_INFO:
        Macros.type(PSTR("Keyboardio Atreus - Kaleidoscope "));
        Macros.type(PSTR(BUILD_INFORMATION));
        break;
      default:
        break;
    }
  }
  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.setup();
  EEPROMKeymap.setup(9);
  Kaleidoscope.serialPort().begin(9600);
  DynamicMacros.reserve_storage(64);
  LayerNames.reserve_storage(128);
  Layer.move(EEPROMSettings.default_layer());
}

void loop() {
  Kaleidoscope.loop();
}
