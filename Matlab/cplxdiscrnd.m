%==========================================================================
%
% VersIcaL impedance bridge data analysis
%
% Copyright 2020	Massimo Ortolano <massimo.ortolano@polito.it> 
%                	Martina Marzano <m.marzano@inrim.it>
%
% This code is licensed under MIT license (see LICENSE.txt for details)
%
%==========================================================================

function Z = cplxdiscrnd(C,R,varargin)
%CPLXDISCRND Random arrays from continuous uniform distribution over a
%complex disk
Z = C+R*sqrt(unifrnd(0,1,varargin{:})).*exp(1i*unifrnd(0,2*pi,varargin{:}));
end