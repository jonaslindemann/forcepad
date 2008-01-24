## Copyright (C) 2007 Jonas
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

## plani4_test

## Author: Jonas <Jonas@RA>
## 
## 2007-12-26 Jonas <Jonas@RA>
## * Initial revision

E = 2.1e9;
v = 0.3;
t = 0.1;
ptype = 1;
ir = 3;

ep = [ptype t ir];

D = hooke(ptype, E, v);

ex = [0.0 1.0 1.0 0.0];
ey = [0.0 0.0 1.0 1.0];

Ke = plani4e(ex, ey, ep, D)