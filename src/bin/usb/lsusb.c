/***************************************************************************
 *   Copyright (C) 2004 by Ralf Kaestner                                   *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>

#include "config/parser.h"
#include "usb/usb.h"

int main(int argc, char **argv) {
  config_parser_t parser;
  int i;

  config_parser_init_default(&parser, "List USB devices",
    "The command displays lsusb-like information about USB buses in "
    "the system and the devices connected to them.");
  config_parser_parse(&parser, argc, argv, config_parser_exit_error);
  
  usb_context_init(usb_default_context);
  if (usb_default_context->num_devices) {
    for (i = 0; i < usb_default_context->num_devices; ++i)
      usb_print(stdout, &usb_default_context->devices[i]);
  }
  else
    fprintf(stdout, "No devices found.\n");

  usb_context_release(usb_default_context);
  return 0;
}
