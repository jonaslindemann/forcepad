## Copyright (C) 2008 Jonas
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

## system_test

## Author: Jonas <Jonas@RA>
## 
## 2008-01-22 Jonas <Jonas@RA>
## * Initial revision

clear all

calfem

E=2.1e9;
v=0.35;
t=0.1;
ptype=1;
ep=[ptype t 1];
ex = [0 0.25 0.25 0];
ey = [0 0 0.25 0.25];

D=hooke(ptype,E,v);
Ke=plani4e(ex, ey, ep, D);

nElements = size(topo,1);
nDofs = max(max(topo));

negativeValues = find(topo<0);
negativeCount = size(negativeValues,1);

topo(negativeValues) = nDofs+1;

edof = [ zeros(nElements,1) topo];

Kp=zeros(nDofs+1, nDofs+1);

for i=1:nElements
  Kp=assem(edof(i,:),Kp,Ke);
end;

Kp(nDofs+1,:) = []
Kp(:,nDofs+1) = []