//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2006 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se
//

#ifndef forcepad_config
#define forcepad_config

#define FORCEPAD_NEW_UI

#define FORCEPAD_VERSION_MAJOR 2
#define FORCEPAD_VERSION_MINOR 4
#define FORCEPAD_VERSION_RELEASE 0

#ifndef FORCEPAD_RIGID
#define FORCEPAD_VERSION_STRING "ForcePAD version 2.4.0-BETA-3"
#ifdef FORCEPAD_NEW_UI
#define FORCEPAD_NAME "ForcePAD 2 - (2.4.0-BETA-3)"
#else
#define FORCEPAD_NAME "ForcePAD 2"
#endif
#else
#define FORCEPAD_VERSION_STRING "ForcePAD-R version 2.3.0"
#define FORCEPAD_NAME "ForcePAD/R 2"
#endif

#endif
