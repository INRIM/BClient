% VersIcaL impedance bridge data analysis
%
% Copyright 2020	Massimo Ortolano <massimo.ortolano@polito.it> 
%                	Martina Marzano <m.marzano@inrim.it>
%
% This code is licensed under MIT license (see LICENSE.txt for details)

function bridgeParameters = ImportBridgeData(bridgeFile)
global isOctave results

% Check bridge file status
[bridgeFileStatus,bridgeList] = xlsfinfo(bridgeFile);
if isempty(bridgeFileStatus)
    error('Cannot read the bridge data file');
end

if isOctave
    bridgeList(:,2) = [];
end

% Select bridge sheet and import parameters
generatorSpecificationsSheet = ismember(bridgeList, 'GeneratorSpecifications');
if ~any(generatorSpecificationsSheet)
    error('GeneratorSpecifications sheet missing in the bridge data file');
end
bridgeList(generatorSpecificationsSheet) = [];

[selectedBridgeIdx,isOk] = listdlg('ListString', bridgeList,...
    'SelectionMode','Single','Name','Select bridge parameters sheet','ListSize',[300,200]);
if ~isOk
    bridgeParameters = [];
    return
end

[~,bridgeParameters.transformerParameter,~] = xlsread(bridgeFile,bridgeList{selectedBridgeIdx},'B2');
switch bridgeParameters.transformerParameter{1}
    case 'n'
        bridgeParameters.highBalanceDetection = 'Current';
    case 'Zm'
        bridgeParameters.highBalanceDetection = 'Voltage';
    otherwise
        error('Unknown transformer parameter');
end

[bridgeParameters.values1,bridgeParameters.text1,~] = xlsread(bridgeFile, bridgeList{selectedBridgeIdx},'A3:L1036');

% Select frequency row
dlgName = sprintf('Select frequency [%s]',bridgeList{selectedBridgeIdx});
[selectedFrequencyIdx1,isOk] = listdlg('ListString', cellstr(num2str(bridgeParameters.values1(:,1))),...
    'SelectionMode', 'Single','Name',dlgName,'ListSize',[400,200],...
    'PromptString', 'Frequency');
if ~isOk
    bridgeParameters = [];
    return
end

eval(['bridgeParameters.' bridgeParameters.transformerParameter{1} '=' bridgeParameters.text1{selectedFrequencyIdx1,1} ';']);
bridgeParameters.z1 = eval(bridgeParameters.text1{selectedFrequencyIdx1,2});
bridgeParameters.z2 = eval(bridgeParameters.text1{selectedFrequencyIdx1,3});
bridgeParameters.CL1 = bridgeParameters.values1(selectedFrequencyIdx1,5)*1e-12;
bridgeParameters.uncCL1 = bridgeParameters.values1(selectedFrequencyIdx1,6)*1e-12;
bridgeParameters.CL2 = bridgeParameters.values1(selectedFrequencyIdx1,7)*1e-12;
bridgeParameters.uncCL2 = bridgeParameters.values1(selectedFrequencyIdx1,8)*1e-12;
bridgeParameters.CH1 = bridgeParameters.values1(selectedFrequencyIdx1,9)*1e-12;
bridgeParameters.uncCH1 = bridgeParameters.values1(selectedFrequencyIdx1,10)*1e-12;
bridgeParameters.CH2 = bridgeParameters.values1(selectedFrequencyIdx1,11)*1e-12;
bridgeParameters.uncCH2 = bridgeParameters.values1(selectedFrequencyIdx1,12)*1e-12;

% Import bridge nonlinearity and crosstalk
[bridgeParameters.values2,bridgeParameters.text2,~] = xlsread(bridgeFile, 'GeneratorSpecifications','A4:CK1037');
[selectedFrequencyIdx2,isOk] = listdlg('ListString', cellstr(num2str(bridgeParameters.values2)),...
    'SelectionMode', 'Single','Name','Select frequency [GeneratorSpecifications]','ListSize',[400,200],...
    'PromptString', 'Frequency');
if ~isOk
    bridgeParameters = [];
    return
end

bridgeParameters.DeltaWnl = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,1} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,23} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,45} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,67} ']']); 
bridgeParameters.uDeltaWnl = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,2} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,24} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,46} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,68} ']']);
bridgeParameters.E10 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,3} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,25} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,47} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,69} ']']);
bridgeParameters.uE10 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,4} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,26} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,48} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,70} ']']);
bridgeParameters.E20 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,5} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,27} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,49} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,71} ']']);
bridgeParameters.uE20 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,6} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,28} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,50} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,72} ']']);
bridgeParameters.a12 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,7} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,29} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,51} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,73} ']']);
bridgeParameters.ua12 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,8} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,30} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,52} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,74} ']']);
bridgeParameters.a13 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,9} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,31} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,53} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,75} ']']);
bridgeParameters.ua13 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,10} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,32} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,54} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,76} ']']);
bridgeParameters.a14 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,11} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,33} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,55} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,77} ']']);
bridgeParameters.ua14 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,12} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,34} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,56} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,78} ']']);
bridgeParameters.a15 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,13} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,35} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,57} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,79} ']']);
bridgeParameters.ua15 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,14} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,36} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,58} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,80} ']']);

bridgeParameters.a21 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,15} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,37} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,59} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,81} ']']);
bridgeParameters.ua21 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,16} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,38} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,60} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,82} ']']);
bridgeParameters.a23 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,17} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,39} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,61} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,83} ']']);
bridgeParameters.ua23 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,18} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,40} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,62} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,84} ']']);
bridgeParameters.a24 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,19} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,41} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,63} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,85} ']']);
bridgeParameters.ua24 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,20} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,42} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,64} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,86} ']']);
bridgeParameters.a25 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,21} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,43} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,65} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,87} ']']);
bridgeParameters.ua25 = eval(['[' bridgeParameters.text2{selectedFrequencyIdx2,22} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,44} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,66} ';'...
    bridgeParameters.text2{selectedFrequencyIdx2,88} ']']);
end