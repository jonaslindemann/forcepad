%%%% A 99 LINE TOPOLOGY OPTIMIZATION CODE BY OLE SIGMUND, JANUARY 2000 %%%
%%%% CODE MODIFIED FOR INCREASED SPEED, September 2002, BY OLE SIGMUND %%%
function [loop]=top_new(nelx,nely,volfrac,penal,rmin,filter,force);  %Indtast: top_new(100,50,0.5,3.0,2.75,2,1)
% INITIALIZE
% filter=1: Ole Sigmund filter     
% filter=2: Back-Pedersen filter
% force=1: Simpel underst�ttet bj�lke
% force=2: Fast indsp�ndt bj�lke
% clc

% penal: Penalty parameter on density

% Output start
fig=figure;
set(fig,'DoubleBuffer','on');
set(gca,'xlim',[-80 80],'ylim',[-80 80],...
       	   'NextPlot','replace','Visible','off')
mov = avifile('example.avi')
%
 
x(1:nely,1:nelx) = volfrac;      % x contains the density for each element
                                 % alle x tildeles en v�rdi svarende til volfrac ex 0.5 
loop = 0; 
change = 1.;    % Defininition of max change from previous density
H=subplot(2,1,1)

% START ITERATION
while (change > 0.010) & (loop <100)   
  loop = loop + 1;
  xold = x;    % den gamle x-v�rdi erstattes af en ny
% FE-ANALYSIS
if force==1
  [U]=FE_simpel_understoettet(nelx,nely,x,penal);                                                % flytningen beregnes vha. FEM
elseif force==2
  [U]=FE_indspaendt(nelx,nely,x,penal);
end

% OBJECTIVE FUNCTION AND SENSITIVITY ANALYSIS
  [KE] = lk;                                                                % lokal stivhedsmatrice
  c = 0.;
  for ely = 1:nely
    for elx = 1:nelx
      n1 = (nely+1)*(elx-1)+ely; 
      n2 = (nely+1)* elx   +ely;
      Ue = U([2*n1-1;2*n1; 2*n2-1;2*n2; 2*n2+1;2*n2+2; 2*n1+1;2*n1+2],1); % Local element displacements
      c = c + x(ely,elx)^penal*Ue'*KE*Ue;                                   % objective function ligning 1.17
      dc(ely,elx) = -penal*x(ely,elx)^(penal-1)*Ue'*KE*Ue;                  % d(c)/d(rho)
      % dc is the sensitivity for each element
    end
  end
  
% FILTERING OF SENSITIVITIES
if filter==1
   [dc]=Ole_Sigmund(nelx,nely,rmin,x,dc);
elseif filter==2
   [dc]=Back_Pedersen(nelx,nely,rmin,dc);
end
     
% DESIGN UPDATE BY THE OPTIMALITY CRITERIA METHOD
  [x]    = OC(nelx,nely,x,volfrac,dc);               % x har v�rdien mellem 0 og 1, ben�vnes [rho_e]
  
% PRINT RESULTS
  change = max(max(abs(x-xold)));
   disp([' It.: ' sprintf('%4i',loop) ' Obj.: ' sprintf('%10.4f',c) ...
       ' Vol.: ' sprintf('%6.3f',sum(sum(x))/(nelx*nely)) ...
         ' ch.: ' sprintf('%6.3f',change )])
     
% % PLOT DENSITIES  only output
Npic = max(1,25-loop);
for j=0:Npic    
  subplot(2,1,1);
  xmid = xold*((Npic-j)/Npic)+x*j/Npic;
  colormap(colormap); image(xmid*64); axis equal; axis tight; axis off;pause(1e-6);
  subplot(2,1,2);
  xdif = x-xold;
  colormap(colormap); image(5*xdif*64+32); axis equal; axis tight; axis off;pause(1e-6);

  F = getframe(gca);

  F = getframe(fig);
  mov = addframe(mov,F);
end
% end output


end % End while loop
       
mov = close(mov);
 

%%%%%%%%%% OPTIMALITY CRITERIA UPDATE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [xnew]=OC(nelx,nely,x,volfrac,dc)  
l1 = 0; l2 = 100000; move = 0.2;
while (l2-l1 > 1e-4)
  lmid = 0.5*(l2+l1);
  xnew = max(0.001,max(x-move,min(1.,min(x+move,x.*sqrt(-dc./lmid)))));
  if sum(sum(xnew)) - volfrac*nelx*nely > 0;                                % stopper l�kken n�r volumereduktionen er opn�et
    l1 = lmid;
  else
    l2 = lmid;
  end
end
%%%%%%%%%% MESH-INDEPENDENCY FILTER %%%%%%%%%%%% ligning 1.27, side 35 %%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [dcn]=Ole_Sigmund(nelx,nely,rmin,x,dc)                                   % ved rmin<1 deaktiveres filtreringen
dcn=zeros(nely,nelx);
for i = 1:nelx
  for j = 1:nely
    sum=0.0; 
    for k = max(i-floor(rmin),1):min(i+floor(rmin),nelx)
      for l = max(j-floor(rmin),1):min(j+floor(rmin),nely)
        fac = rmin-sqrt((i-k)^2+(j-l)^2);
        sum = sum+max(0,fac);
        dcn(j,i) = dcn(j,i) + max(0,fac)*x(l,k)*dc(l,k);
      end
    end
    dcn(j,i) = dcn(j,i)/(x(j,i)*sum);
  end
end

function [dcn]=Back_Pedersen(nelx,nely,rmin,dc)         
A(1:nely, 1:nelx)=1;  % Volumen for each element
dcn=zeros(nely,nelx);
for i = 1:nelx
  for j = 1:nely
    sum=0.0; 
    for k = max(i-floor(rmin),1):min(i+floor(rmin),nelx);
      for l = max(j-floor(rmin),1):min(j+floor(rmin),nely);
        fac = rmin-sqrt((i-k)^2+(j-l)^2);
        sum = sum+max(0,fac)*A(l,k);
        dcn(j,i) = dcn(j,i) + max(0,fac)*dc(l,k);
      end
    end
    dcn(j,i) = dcn(j,i)/sum;% FE-ANALYSIS

  end
end

%%%%%%%%%% FE-ANALYSIS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
function [U]=FE_simpel_understoettet(nelx,nely,x,penal)                                          % Q4 element
[KE] = lk; 
K = sparse(2*(nelx+1)*(nely+1), 2*(nelx+1)*(nely+1));
F = sparse(2*(nely+1)*(nelx+1),1); 
U = zeros(2*(nely+1)*(nelx+1),1);
for elx = 1:nelx
  for ely = 1:nely
    n1 = (nely+1)*(elx-1)+ely; 
    n2 = (nely+1)* elx   +ely;
    edof = [2*n1-1; 2*n1; 2*n2-1; 2*n2; 2*n2+1; 2*n2+2; 2*n1+1; 2*n1+2];
    K(edof,edof) = K(edof,edof) + x(ely,elx)^penal*KE;
  end
end

% DEFINE LOADS AND SUPPORTS (HALF MBB-BEAM)
F(2,1) = -1;
fixeddofs   = union([1:2:2*(nely+1)],[2*(nelx+1)*(nely+1)]);
alldofs     = [1:2*(nely+1)*(nelx+1)];
freedofs    = setdiff(alldofs,fixeddofs);
% SOLVING
U(freedofs,:) = K(freedofs,freedofs) \ F(freedofs,:);      
U(fixeddofs,:)= 0;

function [U]=FE_indspaendt(nelx,nely,x,penal)                                          % Q4 element
[KE] = lk; 
K = sparse(2*(nelx+1)*(nely+1), 2*(nelx+1)*(nely+1));
F = sparse(2*(nely+1)*(nelx+1),1); 
U = zeros(2*(nely+1)*(nelx+1),1);
for elx = 1:nelx
  for ely = 1:nely
    n1 = (nely+1)*(elx-1)+ely; 
    n2 = (nely+1)* elx   +ely;
    edof = [2*n1-1; 2*n1; 2*n2-1; 2*n2; 2*n2+1; 2*n2+2; 2*n1+1; 2*n1+2];
    K(edof,edof) = K(edof,edof) + x(ely,elx)^penal*KE;
  end
end
% DEFINE LOADS AND SUPPORTS (HALF MBB-BEAM)
F(2*(nely+1)*(nelx+1)-(nely) ,1) = -1;
fixeddofs   = [1:1:2*(nely+1)];
alldofs     = [1:2*(nely+1)*(nelx+1)];
freedofs    = setdiff(alldofs,fixeddofs);
% SOLVING
U(freedofs,:) = K(freedofs,freedofs) \ F(freedofs,:);      
U(fixeddofs,:)= 0;


%%%%%%%%%% ELEMENT STIFFNESS MATRIX %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [KE]=lk
E = 1.; 
nu = 0.3;
k=[11/24-1/8*nu, 1/8+1/8*nu, -5/24-1/8*nu, -1/8+3/8*nu, -7/24+1/8*nu, -1/8-1/8*nu ...  
1/24+1/8*nu,   1/8-3/8*nu];
% k=[ 1/2-nu/6   1/8+nu/8 -1/4-nu/12 -1/8+3*nu/8 ... 
%   -1/4+nu/12 -1/8-nu/8  nu/6       1/8-3*nu/8];
KE = E/(1-nu^2)*[ k(1) k(2) k(3) k(4) k(5) k(6) k(7) k(8)
                  k(2) k(1) k(8) k(7) k(6) k(5) k(4) k(3)
                  k(3) k(8) k(1) k(6) k(7) k(4) k(5) k(2)
                  k(4) k(7) k(6) k(1) k(8) k(3) k(2) k(5)
                  k(5) k(6) k(7) k(8) k(1) k(2) k(3) k(4)
                  k(6) k(5) k(4) k(3) k(2) k(1) k(8) k(7)
                  k(7) k(4) k(5) k(2) k(3) k(8) k(1) k(6)
                  k(8) k(3) k(2) k(5) k(4) k(7) k(6) k(1)];
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This Matlab code was written by Ole Sigmund, Department of Solid         %
% Mechanics, Technical University of Denmark, DK-2800 Lyngby, Denmark.     %
% Please sent your comments to the author: sigmund@fam.dtu.dk              %
%                                                                          %
% The code is intended for educational purposes and theoretical details    %
% are discussed in the paper                                               %
% "A 99 line topology optimization code written in Matlab"                 %
% by Ole Sigmund (2001), Structural and Multidisciplinary Optimization,    %
% Vol 21, pp. 120--127.                                                    %
%                                                                          %
% The code as well as a postscript version of the paper can be             %
% downloaded from the web-site: http://www.topopt.dtu.dk                   %
%                                                                          %
% Disclaimer:                                                              %
% The author reserves all rights but does not guaranty that the code is    %
% free from errors. Furthermore, he shall not be liable in any event       %
% caused by the use of the program.                                        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
