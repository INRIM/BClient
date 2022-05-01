% VersIcaL impedance bridge data analysis
%
% Copyright 2020-2022	Massimo Ortolano <massimo.ortolano@polito.it> 
%                	    Martina Marzano <m.marzano@inrim.it>
%
% This code is licensed under MIT license (see LICENSE.txt for details)

function measurementData = ImportMeasurementData(dataFile)

dataFileID = fopen(dataFile,'r');
dataHeader = textscan(dataFileID,[repmat('%s\t',1,38),'%s'],1);
rawData = textscan(dataFileID,['%s\t%s\t%f\t' repmat('%f\t',1,35) '%f']);
fclose(dataFileID);

timeStampIdx = ismember([dataHeader{:}],'Timestamp');
frequencyIdx = ismember([dataHeader{:}],'Frequency');
modeIdx = find(ismember([dataHeader{:}],'Mode'));

% Measurement times
measurementData.timeStamp = rawData{timeStampIdx};

% Measurement frequency
measurementData.fRead = rawData{frequencyIdx};

% Voltages 
E1RealIdx = ismember([dataHeader{:}],'Re(E1)');
E1ImagIdx = ismember([dataHeader{:}],'Im(E1)');
E2RealIdx = ismember([dataHeader{:}],'Re(E2)');
E2ImagIdx = ismember([dataHeader{:}],'Im(E2)');
E3RealIdx = ismember([dataHeader{:}],'Re(E3)');
E3ImagIdx = ismember([dataHeader{:}],'Im(E3)');
E4RealIdx = ismember([dataHeader{:}],'Re(E4)');
E4ImagIdx = ismember([dataHeader{:}],'Im(E4)');
E5RealIdx = ismember([dataHeader{:}],'Re(E5)');
E5ImagIdx = ismember([dataHeader{:}],'Im(E5)');

E1RangeIdx = ismember([dataHeader{:}],'Range(E1)');
E2RangeIdx = ismember([dataHeader{:}],'Range(E2)');
E3RangeIdx = ismember([dataHeader{:}],'Range(E3)');
E4RangeIdx = ismember([dataHeader{:}],'Range(E4)');
E5RangeIdx = ismember([dataHeader{:}],'Range(E5)');

E1ThresholdIdx = ismember([dataHeader{:}],'Threshold(E1)');
E2ThresholdIdx = ismember([dataHeader{:}],'Threshold(E2)');
E3ThresholdIdx = ismember([dataHeader{:}],'Threshold(E3)');
E4ThresholdIdx = ismember([dataHeader{:}],'Threshold(E4)');
ELThresholdIdx = ismember([dataHeader{:}],'Threshold(E5)');

DLARealIdx = ismember([dataHeader{:}],'Re(DLA)');
DLAImagIdx = ismember([dataHeader{:}],'Im(DLA)');
DLBRealIdx = ismember([dataHeader{:}],'Re(DLB)');
DLBImagIdx = ismember([dataHeader{:}],'Im(DLB)');

DHARealIdx = ismember([dataHeader{:}],'Re(DHA)');
DHAImagIdx = ismember([dataHeader{:}],'Im(DHA)');
DHBRealIdx = ismember([dataHeader{:}],'Re(DHB)');
DHBImagIdx = ismember([dataHeader{:}],'Im(DHB)');

measurementData.E1Read = rawData{E1RealIdx}+1i*rawData{E1ImagIdx};
measurementData.E2Read = rawData{E2RealIdx}+1i*rawData{E2ImagIdx};
measurementData.E3Read = rawData{E3RealIdx}+1i*rawData{E3ImagIdx};
measurementData.E4Read = rawData{E4RealIdx}+1i*rawData{E4ImagIdx};
measurementData.E5Read = rawData{E5RealIdx}+1i*rawData{E5ImagIdx};

measurementData.E1Range = uint8(rawData{E1RangeIdx});
measurementData.E2Range = uint8(rawData{E2RangeIdx});
measurementData.E3Range = uint8(rawData{E3RangeIdx});
measurementData.E4Range = uint8(rawData{E4RangeIdx});
measurementData.E5Range = uint8(rawData{E5RangeIdx});

measurementData.E1Threshold = rawData{E1ThresholdIdx};
measurementData.E2Threshold = rawData{E2ThresholdIdx};
measurementData.E3Threshold = rawData{E3ThresholdIdx};
measurementData.E4Threshold = rawData{E4ThresholdIdx};
measurementData.ELThreshold = rawData{ELThresholdIdx};

measurementData.VLA = rawData{DLARealIdx}+1i*rawData{DLAImagIdx};
measurementData.VLB = rawData{DLBRealIdx}+1i*rawData{DLBImagIdx};
measurementData.DHA = rawData{DHARealIdx}+1i*rawData{DHAImagIdx};
measurementData.DHB = rawData{DHBRealIdx}+1i*rawData{DHBImagIdx};

measurementData.forwardIdx = find(ismember([rawData{modeIdx}],'Forward'));
measurementData.reverseIdx = find(ismember([rawData{modeIdx}],'Reverse'));    

N = numel(measurementData.forwardIdx); % Number of measurements
if N~=numel(measurementData.reverseIdx)
    error('Uneven number of forward and reverse measurements');
end
measurementData.N = N;

measurementData.E1FRead = measurementData.E1Read(measurementData.forwardIdx);
measurementData.E1RRead = measurementData.E1Read(measurementData.reverseIdx);
measurementData.E2FRead = measurementData.E2Read(measurementData.forwardIdx);
measurementData.E2RRead = measurementData.E2Read(measurementData.reverseIdx);
measurementData.E3FRead = measurementData.E3Read(measurementData.forwardIdx);
measurementData.E3RRead = measurementData.E3Read(measurementData.reverseIdx);
measurementData.E4FRead = measurementData.E4Read(measurementData.forwardIdx);
measurementData.E4RRead = measurementData.E4Read(measurementData.reverseIdx);
measurementData.E5FRead = measurementData.E5Read(measurementData.forwardIdx);
measurementData.E5RRead = measurementData.E5Read(measurementData.reverseIdx);

measurementData.E1FReadMean = mean(measurementData.E1FRead);
measurementData.E1RReadMean = mean(measurementData.E1RRead);
measurementData.E2FReadMean = mean(measurementData.E2FRead);
measurementData.E2RReadMean = mean(measurementData.E2RRead);
measurementData.E3FReadMean = mean(measurementData.E3FRead);
measurementData.E3RReadMean = mean(measurementData.E3RRead);
measurementData.E4FReadMean = mean(measurementData.E4FRead);
measurementData.E4RReadMean = mean(measurementData.E4RRead);
measurementData.E5FReadMean = mean(measurementData.E5FRead);
measurementData.E5RReadMean = mean(measurementData.E5RRead);

measurementData.E1FThreshold = measurementData.E1Threshold(measurementData.forwardIdx);
measurementData.E1RThreshold = measurementData.E1Threshold(measurementData.reverseIdx);
measurementData.E2FThreshold = measurementData.E2Threshold(measurementData.forwardIdx);
measurementData.E2RThreshold = measurementData.E2Threshold(measurementData.reverseIdx);
measurementData.E3FThreshold = measurementData.E3Threshold(measurementData.forwardIdx);
measurementData.E3RThreshold = measurementData.E3Threshold(measurementData.reverseIdx);
measurementData.E4FThreshold = measurementData.E4Threshold(measurementData.forwardIdx);
measurementData.E4RThreshold = measurementData.E4Threshold(measurementData.reverseIdx);
measurementData.ELFThreshold = measurementData.ELThreshold(measurementData.forwardIdx);
measurementData.ELRThreshold = measurementData.ELThreshold(measurementData.reverseIdx);

measurementData.E1FRange = measurementData.E1Range(measurementData.forwardIdx);
measurementData.E1RRange = measurementData.E1Range(measurementData.reverseIdx);
measurementData.E2FRange = measurementData.E2Range(measurementData.forwardIdx);
measurementData.E2RRange = measurementData.E2Range(measurementData.reverseIdx);
measurementData.E3FRange = measurementData.E3Range(measurementData.forwardIdx);
measurementData.E3RRange = measurementData.E3Range(measurementData.reverseIdx);
measurementData.E4FRange = measurementData.E4Range(measurementData.forwardIdx);
measurementData.E4RRange = measurementData.E4Range(measurementData.reverseIdx);
measurementData.E5FRange = measurementData.E5Range(measurementData.forwardIdx);
measurementData.E5RRange = measurementData.E5Range(measurementData.reverseIdx);

measurementData.VLAF = measurementData.VLA(measurementData.forwardIdx);
measurementData.VLAR = measurementData.VLA(measurementData.reverseIdx);
measurementData.VLBF = measurementData.VLB(measurementData.forwardIdx);
measurementData.VLBR = measurementData.VLB(measurementData.reverseIdx);

measurementData.DHAF = measurementData.DHA(measurementData.forwardIdx);
measurementData.DHAR = measurementData.DHA(measurementData.reverseIdx);
measurementData.DHBF = measurementData.DHB(measurementData.forwardIdx);
measurementData.DHBR = measurementData.DHB(measurementData.reverseIdx);

measurementData.DHAFMean = mean(measurementData.DHAF);
measurementData.DHARMean = mean(measurementData.DHAR);
measurementData.DHBFMean = mean(measurementData.DHBF);
measurementData.DHBRMean = mean(measurementData.DHBR);
end