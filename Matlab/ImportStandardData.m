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

global standardParameters

%% Check standard file status
[standardFileStatus,standardList] = xlsfinfo(standardFile);
if isempty(standardFileStatus)
    clear isOctave standardFile standardFileStatus standardList
    error('Cannot read the standard data file');
end
clear standardFileStatus
if isOctave
    standardList(:,2) = [];
end


%% Select standard and import parameters
standardCount = numel(standardList);
standardProperties = cell(1,standardCount);
for stdIdx=1:standardCount
    [~,standardProperties{stdIdx}] = xlsread(standardFile,standardList{stdIdx},'B1:B3');
end

standardListLabels = cell(1,standardCount);
for stdIdx=1:standardCount
    standardListLabels{stdIdx} = sprintf('[%s] %s %s %s',standardList{stdIdx},...
    standardProperties{stdIdx}{1}, standardProperties{stdIdx}{2}, standardProperties{stdIdx}{3});
end

[selectedStandardIdx,isOk] = listdlg('ListString', standardListLabels,...
    'SelectionMode','Single','Name','Select standard','ListSize',[320,300]);
clear standardCount standardListLabels stdIdx
if ~isOk
    clear isOctave standardList selectedStandard standardProperties isOk
    return
end
standardParameters.standardType = standardProperties{selectedStandardIdx}{1};

[standardParameters.values,standardParameters.text,~] = xlsread(standardFile, standardList{selectedStandardIdx},'A8:J1036');

clear standardList selectedStandardIdx standardProperties

%% Select standard parameters row
standardParametersCount = size(standardParameters.values,1);
standardParametersLabels = cell(1,standardParametersCount);
if isOctave
    for parameterIdx=1:standardParametersCount
        standardParametersLabels{parameterIdx} = sprintf('%s   %.0f Hz',...
            datestr(standardParameters.values(parameterIdx,1)+datenum(1900,1,1)-2,24),...
            standardParameters.values(parameterIdx,2));
    end
else
    for parameterIdx=1:standardParametersCount
        standardParametersLabels{parameterIdx} = sprintf('%s            %.0f Hz',standardParameters.text{parameterIdx,1},...
            standardParameters.values(parameterIdx,1));
    end
end
[standardParameters.selectedParametersIdx,isOk] = listdlg('ListString', standardParametersLabels,...
    'SelectionMode', 'Single','Name','Select standard parameters','ListSize',[300,300],...
    'PromptString', 'Calibration date     Frequency');
clear parameterIdx standardParametersCount standardParametersLabels
if ~isOk
    clear isOctave selectedStandard isOk selectedParametersIdx standardParametersCount standardParametersLabels
    return
end
clear isOk