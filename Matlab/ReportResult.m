% VersIcaL impedance bridge data analysis
%
% Copyright 2020-2022	Massimo Ortolano <massimo.ortolano@polito.it> 
%                	    Martina Marzano <m.marzano@inrim.it>
%
% This code is licensed under MIT license (see LICENSE.txt for details)

formatList = {'Ls, Rs', 'Ls, Q', 'Cp, Gp', 'Cp, D', 'Cs, Rs', 'Rs, tau', 'R, X', 'G, B'};

[selectedFormat,isOk] = listdlg('ListString', formatList,...
    'SelectionMode','Single','Name','Select output format','ListSize',[320,300]);
if ~isOk
    clear isOctave isOk selectedFormatIdx formatList
    return
end

switch selectedFormat
    case 1
        mc.LsSample = imag(mc.Z2Sample)/(2*pi*f);
        mc.RsSample = real(mc.Z2Sample);
        fprintf('Ls = ');
        qprintf(mean(mc.LsSample),std(mc.LsSample),'H');
        fprintf('\n');
        fprintf('Rs = ');
        qprintf(mean(mc.RsSample),std(mc.RsSample),'ohm');
        fprintf('\n');
    case 2                        
        mc.LsSample = imag(mc.Z2Sample)/(2*pi*f);
        mc.QSample = abs(imag(mc.Z2Sample)./real(mc.Z2Sample));
        fprintf('Ls = ');
        qprintf(mean(mc.LsSample),std(mc.LsSample),'H');
        fprintf('\n');
        fprintf('Q = ');
        qprintf(mean(mc.QSample),std(mc.QSample));
        fprintf('\n');
    case 3
        mc.CpSample = imag(mc.Y2Sample)/(2*pi*f);
        mc.GpSample = real(mc.Y2Sample);
        fprintf('Cp = ');
        qprintf(mean(mc.CpSample),std(mc.CpSample),'F');
        fprintf('\n');
        fprintf('Gp = ');
        qprintf(mean(mc.GpSample),std(mc.GpSample),'S');
        fprintf('\n');       
    case 4
        mc.CpSample = imag(mc.Y2Sample)/(2*pi*f);
        mc.DSample = abs(real(mc.Y2Sample)./imag(mc.Y2Sample));
        fprintf('Cp = ');
        qprintf(mean(mc.CpSample),std(mc.CpSample),'F');
        fprintf('\n');
        fprintf('D = ');
        qprintf(mean(mc.DSample),std(mc.DSample));
        fprintf('\n');        
    case 5
        mc.CsSample = -1./(2*pi*f*imag(mc.Z2Sample));
        mc.RsSample = real(mc.Z2Sample);
        fprintf('Cs = ');
        qprintf(mean(mc.CsSample),std(mc.CsSample),'F');
        fprintf('\n');
        fprintf('Rs = ');
        qprintf(mean(mc.RsSample),std(mc.RsSample),'ohm');
        fprintf('\n');
    case 6
        mc.RsSample = real(mc.Z2Sample);
        mc.tauSample = imag(mc.Z2Sample)./(2*pi*f*mc.RsSample);
        fprintf('Rs = ');
        qprintf(mean(mc.RsSample),std(mc.RsSample),'ohm');
        fprintf('\n');
        fprintf('τ = ');
        qprintf(mean(mc.tauSample),std(mc.tauSample),'s');
        fprintf('\n');
    case 7        
        mc.RSample = real(mc.Z2Sample);
        mc.XSample = imag(mc.Z2Sample);
        fprintf('R = ');
        qprintf(mean(mc.RSample),std(mc.RSample),'ohm');
        fprintf('\n');
        fprintf('X = ');
        qprintf(mean(mc.XSample),std(mc.XSample),'ohm');
        fprintf('\n');
    case 8        
        mc.GSample = real(mc.Y2Sample);
        mc.BSample = imag(mc.Y2Sample);
        fprintf('G = ');
        qprintf(mean(mc.GSample),std(mc.GSample),'S');
        fprintf('\n');
        fprintf('B = ');
        qprintf(mean(mc.BSample),std(mc.BSample),'S');
        fprintf('\n');        
end

clear isOk selectedFormatIdx formatList