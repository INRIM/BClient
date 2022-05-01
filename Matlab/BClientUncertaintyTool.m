% VersIcaL impedance bridge data analysis
%
% Copyright 2020-2022	Massimo Ortolano <massimo.ortolano@polito.it> 
%                	    Martina Marzano <m.marzano@inrim.it>
%
% This code is licensed under MIT license (see LICENSE.txt for details)

%% Initialise variables
clear variables
format long g
close all

global isOctave measurementData bridgeParameters results f

%% Determine which environment is running
isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0; % From https://stackoverflow.com/a/2246651/2522705

if isOctave
    pkg load io; % To import Excel files, needs also Java Runtime Environment
    pkg load statistics; % Random number generation
    
    % Suppress unwanted warnings
    warning('off', 'Octave:legacy-function');
end

%% Import measurement data
[dataFileName, dataFilePath] = uigetfile('*.txt','Measurement data file');
if dataFileName == 0
    clear isOctave dataFileName dataFilePath
    return;
end
dataFile = [dataFilePath dataFileName];
measurementData = ImportMeasurementData(dataFile);
f = measurementData.fRead(1);
fOutput = inputdlg({'Operating frequency'},...
              'Input', [1 50], {num2str(f)});   
f = str2double(fOutput);
clear fOutput

standardFilePath = dataFilePath;
clear dataFileName dataFilePath

%% Determine impedance ratio and type A uncertainty
results.WFRead = -measurementData.E1FRead./measurementData.E2FRead;
results.WRRead = -measurementData.E2RRead./measurementData.E1RRead;

results.WFReadMean = mean(results.WFRead);
results.WRReadMean = mean(results.WRRead);

A = sqrt(results.WRRead./results.WFRead);
revIdx = real(A) < 0;
A(revIdx) = -A(revIdx);

results.WRead = A.*results.WFRead;

results.WReadMean = mean(results.WRead);
if measurementData.N > 1
    results.WReadUnc = (std(real(results.WRead))+1i*std(imag(results.WRead)))/sqrt(measurementData.N);
    figure
    subplot(211);
    figures.h(1) = plot(1:measurementData.N,real(results.WRead),...
        'o-','Color',[0 0.4470 0.7410],...
        'LineWidth',1.5,'MarkerFaceColor','auto');
    xticks(1:measurementData.N);
    xlabel('Measurement no.');
    ylabel('\Re({\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
    subplot(212);
    figures.h(2) = plot(1:measurementData.N,imag(results.WRead),...
         'o-','Color',[0.8500 0.3250 0.0980],...
         'LineWidth',1.5,'MarkerFaceColor','auto');
    xticks(1:measurementData.N);
    xlabel('Measurement no.');
    ylabel('\Im({\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
    figure
    measurementData.t = max([datenum(measurementData.timeStamp(measurementData.forwardIdx),'yyyymmddTHHMMSS')';... 
        datenum(measurementData.timeStamp(measurementData.reverseIdx),'yyyymmddTHHMMSS')'])';
    measurementData.t = (measurementData.t-measurementData.t(1))*1440;
    subplot(211);
    figures.h(3) = plot(measurementData.t,real(results.WRead),...
        'o-','Color',[0 0.4470 0.7410],...
        'LineWidth',1.5,'MarkerFaceColor','auto');
    xlabel('{\it{}t}/{\rm{}m}','interpreter','tex');
    ylabel('\Re({\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
    subplot(212);
    figures.h(4) = plot(measurementData.t,imag(results.WRead),...
         'o-','Color',[0.8500 0.3250 0.0980],...
         'LineWidth',1.5,'MarkerFaceColor','auto');
    xlabel('{\it{}t}/{\rm{}m}');
    ylabel('\Im({\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
        
    figure
    subplot(211);
    figures.h(5) = plot(1:measurementData.N,real(1./results.WRead),...
        'o-','Color',[0 0.4470 0.7410],...
        'LineWidth',1.5,'MarkerFaceColor','auto');
    xticks(1:measurementData.N);
    xlabel('Measurement no.');
    ylabel('\Re(1/{\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
    subplot(212);
    figures.h(6) = plot(1:measurementData.N,imag(1./results.WRead),...
         'o-','Color',[0.8500 0.3250 0.0980],...
         'LineWidth',1.5,'MarkerFaceColor','auto');
    xticks(1:measurementData.N);
    xlabel('Measurement no.');
    ylabel('\Im(1/{\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
    figure
    subplot(211);
    figures.h(7) = plot(measurementData.t,real(1./results.WRead),...
        'o-','Color',[0 0.4470 0.7410],...
        'LineWidth',1.5,'MarkerFaceColor','auto');
    xlabel('{\it{}t}/{\rm{}m}','interpreter','tex');
    ylabel('\Re(1/{\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
    subplot(212);
    figures.h(8) = plot(measurementData.t,imag(1./results.WRead),...
         'o-','Color',[0.8500 0.3250 0.0980],...
         'LineWidth',1.5,'MarkerFaceColor','auto');
    xlabel('{\it{}t}/{\rm{}m}');
    ylabel('\Im(1/{\it{}W}^{\rm{}read})','interpreter','tex');
    grid on
  
    figure
    figures.h(9) = plot(results.WRead,'+','LineWidth',2);
    xlabel('\Re({\it{}W}^{\rm{}read})','interpreter','tex');
    ylabel('\Im({\it{}W}^{\rm{}read})','interpreter','tex');
    
    figure
    figures.h(10) = plot(1./results.WRead,'+','LineWidth',2);
    xlabel('\Re(1/{\it{}W}^{\rm{}read})','interpreter','tex');
    ylabel('\Im(1/{\it{}W}^{\rm{}read})','interpreter','tex');
else
    results.WReadUnc = NaN();
end

clear A revIdx

%% Import standard data
[standardFileName, standardFilePath] = uigetfile('*.xlsx','Standard data file',standardFilePath);
if standardFileName == 0
    clear isOctave standardFileName standardFilePath
    return;
end
standardFile = [standardFilePath standardFileName];
bridgeFilePath = standardFilePath;
clear standardFileName standardFilePath

run('ImportStandardData');

%% Import bridge data
[bridgeFileName, bridgeFilePath] = uigetfile('*.xlsx','Bridge data file',bridgeFilePath);
if bridgeFileName == 0
    clear isOctave bridgeFileName bridgeFilePath
    return;
end
bridgeFile = [bridgeFilePath bridgeFileName];
saveFilePath = bridgeFilePath;
clear bridgeFileName bridgeFilePath

bridgeParameters = ImportBridgeData(bridgeFile);
if isempty(bridgeParameters)
    clear bridgeFile
    return
end

%% Evaluate uncertainty and report result 
mc = EvaluateUncertainty();
run('ReportResult');

%% Clear temporary variables
clear isOctave

%% Save results
[~,saveFileName,~] = fileparts(dataFile);
[saveFileName, saveFilePath] = uiputfile('*.mat','Save data file',[saveFilePath saveFileName '.mat']);
if saveFileName == 0
    return;
end
saveFile = [saveFilePath saveFileName];
clear saveFilePath saveFileName ans
save(saveFile);