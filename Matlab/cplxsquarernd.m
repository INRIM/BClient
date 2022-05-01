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

function Z = cplxsquarernd(C,A,varargin)
%CPLXSQUARERND Random arrays from continuous uniform distribution over a
%complex square
Z = C+unifrnd(-A,A,varargin{:})+1i*unifrnd(-A,A,varargin{:});
end