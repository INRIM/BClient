%==========================================================================
%
% VersIcaL impedance bridge data analysis
%
% Copyright 2020-2022	Massimo Ortolano <massimo.ortolano@polito.it> 
%                	    Martina Marzano <m.marzano@inrim.it>
%
% This code is licensed under MIT license (see LICENSE.txt for details)
%
%==========================================================================

function mc = EvaluateUncertainty()
global f isOctave measurementData bridgeParameters standardParameters results

sampleSizeOutput = inputdlg({'Enter sample size'},...
              'Sample size', [1 50], {'1e6'});   
mc.sampleSize = str2double(sampleSizeOutput);

parameter1Idx = 3;
parameter2Idx = 5;
parameter3Idx = 7;
if isOctave
    parameter1Idx = parameter1Idx+1;
    parameter2Idx = parameter2Idx+1;
    parameter3Idx = parameter3Idx+1;
end


switch standardParameters.standardType
    case 'R'
        mc.rdc1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter1Idx);
        mc.tau1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter2Idx);
        mc.alpha1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter3Idx);
        
        mc.uncRdc1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter1Idx+1);
        mc.uncTau1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter2Idx+1);
        mc.uncAlpha1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter3Idx+1);
        
        limRdc1Low = mc.rdc1-mc.uncRdc1*sqrt(3);
        limRdc1Up = mc.rdc1+mc.uncRdc1*sqrt(3);
        limTau1Low = mc.tau1-mc.uncTau1*sqrt(3);
        limTau1Up = mc.tau1+mc.uncTau1*sqrt(3);
        limAlpha1Low = mc.alpha1-mc.uncAlpha1*sqrt(3);
        limAlpha1Up = mc.alpha1+mc.uncAlpha1*sqrt(3);
       
        mc.rdc1Sample = unifrnd(limRdc1Low,limRdc1Up,1,mc.sampleSize);
        mc.tau1Sample = unifrnd(limTau1Low,limTau1Up,1,mc.sampleSize);
        mc.alpha1Sample = unifrnd(limAlpha1Low,limAlpha1Up,1,mc.sampleSize);

        mc.Z1Sample = mc.rdc1Sample.*(1+mc.alpha1Sample+2i*pi*f*mc.tau1Sample);
        
    case 'C'
        mc.cp1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter1Idx);
        mc.d1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter2Idx);
        
        mc.uncCp1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter1Idx+1);
        mc.uncD1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter2Idx+1);
        
        limCp1Low = mc.cp1-mc.uncCp1*sqrt(3);
        limCp1Up = mc.cp1+mc.uncCp1*sqrt(3);
        limD1Low = mc.d1-mc.uncD1*sqrt(3);
        limD1Up = mc.d1+mc.uncD1*sqrt(3);
      
        mc.cp1Sample = unifrnd(limCp1Low,limCp1Up,1,mc.sampleSize);
        mc.d1Sample = unifrnd(limD1Low,limD1Up,1,mc.sampleSize);
        
        mc.Y1Sample = 2i*pi*f*mc.cp1Sample.*(1+1i*mc.d1Sample);     
        mc.Z1Sample = 1./mc.Y1Sample;
        
    case 'L'
        mc.ls1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter1Idx);
        mc.rs1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter2Idx);
        
        mc.uncLs1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter1Idx+1);
        mc.uncRs1 = standardParameters.values(standardParameters.selectedParametersIdx,parameter2Idx+1);
        
        limLs1Low = mc.ls1-mc.uncLs1*sqrt(3);
        limLs1Up = mc.ls1+mc.uncLs1*sqrt(3);
        limRs1Low = mc.rs1-mc.uncRs1*sqrt(3);
        limRs1Up = mc.rs1+mc.uncRs1*sqrt(3);
      
        mc.ls1Sample = unifrnd(limLs1Low,limLs1Up,1,mc.sampleSize);
        mc.rs1Sample = unifrnd(limRs1Low,limRs1Up,1,mc.sampleSize);
        
        mc.Z1Sample = mc.rs1Sample + 2i*pi*f*mc.ls1Sample;
end

if measurementData.N > 1
    mc.WSample = normrnd(real(results.WReadMean),real(results.WReadUnc),1,mc.sampleSize)+...
        1i*normrnd(imag(results.WReadMean),imag(results.WReadUnc),1,mc.sampleSize);
else
    WReadSample = results.WRead*ones(1,mc.sampleSize);
    mc.VLPFSample = cplxdiscrnd(0+0i,measurementData.E1FThreshold,1,mc.sampleSize);
    mc.VLPRSample = cplxdiscrnd(0+0i,measurementData.E1RThreshold,1,mc.sampleSize);
    mc.DeltaVLPFSample = cplxdiscrnd(0+0i,measurementData.ELFThreshold,1,mc.sampleSize);
    mc.DeltaVLPRSample = cplxdiscrnd(0+0i,measurementData.ELRThreshold,1,mc.sampleSize);
    mc.CL1Sample = unifrnd(bridgeParameters.CL1-bridgeParameters.uncCL1*sqrt(3),bridgeParameters.CL1+bridgeParameters.uncCL1*sqrt(3),1,mc.sampleSize); 
    mc.CL2Sample = unifrnd(bridgeParameters.CL2-bridgeParameters.uncCL2*sqrt(3),bridgeParameters.CL2+bridgeParameters.uncCL2*sqrt(3),1,mc.sampleSize);
    mc.DeltaWlbSample = (1/2*(1+results.WRead+2*pi*f*(mc.CL1Sample+mc.CL2Sample)*mean(mc.Z1Sample)).*(mc.VLPFSample./measurementData.E1FRead+...
        mc.VLPRSample./measurementData.E2RRead)+...
        1/4*(1-results.WRead+2*pi*f*(mc.CL1Sample-mc.CL2Sample)*mean(mc.Z1Sample)).*(mc.DeltaVLPFSample./measurementData.E1FRead+...
        mc.DeltaVLPRSample./measurementData.E2RRead))*results.WRead;
    switch bridgeParameters.highBalanceDetection
        case 'Current'
            mc.DHAFSample = cplxdiscrnd(0,measurementData.E3FThreshold,1,mc.sampleSize);
            mc.DHARSample = cplxdiscrnd(0,measurementData.E3RThreshold,1,mc.sampleSize);
            mc.DHBFSample = cplxdiscrnd(0,measurementData.E4FThreshold,1,mc.sampleSize);
            mc.DHBRSample = cplxdiscrnd(0,measurementData.E4RThreshold,1,mc.sampleSize);
            mc.DeltaWhbSample = results.WRead*bridgeParameters.n/2*...
                (bridgeParameters.z1*mc.DHAFSample./meaurementData.E1FRead-...
                 bridgeParameters.z2*mc.DHBFSample./meaurementData.E2FRead-...
                 bridgeParameters.z1*mc.DHARSample./meaurementData.E1RRead+...
                 bridgeParameters.z2*mc.DHBRSample./meaurementData.E2RRead);
        case 'Voltage'
            mc.DHAFSample = cplxdiscrnd(0,measurementData.E3FThreshold,1,mc.sampleSize);
            mc.DHARSample = cplxdiscrnd(0,measurementData.E3RThreshold,1,mc.sampleSize);
            mc.DHBFSample = cplxdiscrnd(0,measurementData.E4FThreshold,1,mc.sampleSize);
            mc.DHBRSample = cplxdiscrnd(0,measurementData.E4RThreshold,1,mc.sampleSize);
            mc.DeltaWhbSample = results.WRead/(2*bridgeParameters.Zm)*...
                (bridgeParameters.z1*mc.DHAFSample./measurementData.E1FRead-...
                 bridgeParameters.z2*mc.DHBFSample./measurementData.E2FRead-...
                 bridgeParameters.z1*mc.DHARSample./measurementData.E1RRead+...
                 bridgeParameters.z2*mc.DHBRSample./measurementData.E2RRead);
    end
    mc.WSample = WReadSample+mc.DeltaWlbSample+mc.DeltaWhbSample;
end    

mc.DeltaWnlSample = cplxdiscrnd(bridgeParameters.DeltaWnl(measurementData.E1FRange(1)+1),2*bridgeParameters.uDeltaWnl(measurementData.E1FRange(1)+1),1,mc.sampleSize);

mc.E10Sample = cplxdiscrnd(bridgeParameters.E10(measurementData.E1FRange(1)+1),2*bridgeParameters.uE10(measurementData.E1FRange(1)+1),1,mc.sampleSize);
mc.E20Sample = cplxdiscrnd(bridgeParameters.E20(measurementData.E2FRange(1)+1),2*bridgeParameters.uE20(measurementData.E2FRange(1)+1),1,mc.sampleSize);
mc.a12Sample = cplxdiscrnd(bridgeParameters.a12(measurementData.E1FRange(1)+1),2*bridgeParameters.ua12(measurementData.E1FRange(1)+1),1,mc.sampleSize);
mc.a13Sample = cplxdiscrnd(bridgeParameters.a13(measurementData.E1FRange(1)+1),2*bridgeParameters.ua13(measurementData.E1FRange(1)+1),1,mc.sampleSize);
mc.a14Sample = cplxdiscrnd(bridgeParameters.a14(measurementData.E1FRange(1)+1),2*bridgeParameters.ua14(measurementData.E1FRange(1)+1),1,mc.sampleSize);
mc.a15Sample = cplxdiscrnd(bridgeParameters.a15(measurementData.E1FRange(1)+1),2*bridgeParameters.ua15(measurementData.E1FRange(1)+1),1,mc.sampleSize);
mc.a21Sample = cplxdiscrnd(bridgeParameters.a21(measurementData.E2FRange(1)+1),2*bridgeParameters.ua21(measurementData.E2FRange(1)+1),1,mc.sampleSize);
mc.a23Sample = cplxdiscrnd(bridgeParameters.a23(measurementData.E2FRange(1)+1),2*bridgeParameters.ua23(measurementData.E2FRange(1)+1),1,mc.sampleSize);
mc.a24Sample = cplxdiscrnd(bridgeParameters.a24(measurementData.E2FRange(1)+1),2*bridgeParameters.ua24(measurementData.E2FRange(1)+1),1,mc.sampleSize);
mc.a25Sample = cplxdiscrnd(bridgeParameters.a25(measurementData.E2FRange(1)+1),2*bridgeParameters.ua25(measurementData.E2FRange(1)+1),1,mc.sampleSize);

mc.DeltaWctSample = -1/2*results.WReadMean*...
    (mc.E10Sample.*(1/measurementData.E1FReadMean-1/measurementData.E1RReadMean)+...
     mc.E20Sample.*(1/measurementData.E2FReadMean-1/measurementData.E2RReadMean)+...
     mc.a12Sample.*(results.WRReadMean-1/results.WFReadMean)+...
     mc.a21Sample.*(1/results.WRReadMean-results.WFReadMean)+...
     mc.a13Sample.*(measurementData.E3FReadMean/measurementData.E1FReadMean-measurementData.E3RReadMean/measurementData.E1RReadMean)+...
     mc.a14Sample.*(measurementData.E4FReadMean/measurementData.E1FReadMean-measurementData.E4RReadMean/measurementData.E1RReadMean)+...
     mc.a15Sample.*(measurementData.E5FReadMean/measurementData.E1FReadMean-measurementData.E5RReadMean/measurementData.E1RReadMean)+...
     mc.a23Sample.*(measurementData.E3FReadMean/measurementData.E2FReadMean-measurementData.E3RReadMean/measurementData.E2RReadMean)+...
     mc.a24Sample.*(measurementData.E4FReadMean/measurementData.E2FReadMean-measurementData.E4RReadMean/measurementData.E2RReadMean)+...
     mc.a25Sample.*(measurementData.E5FReadMean/measurementData.E2FReadMean-measurementData.E5RReadMean/measurementData.E2RReadMean));

mc.CH1Sample = unifrnd(bridgeParameters.CH1-bridgeParameters.uncCH1*sqrt(3),bridgeParameters.CH1+bridgeParameters.uncCH1*sqrt(3),1,mc.sampleSize); 
mc.CH2Sample = unifrnd(bridgeParameters.CH2-bridgeParameters.uncCH2*sqrt(3),bridgeParameters.CH2+bridgeParameters.uncCH2*sqrt(3),1,mc.sampleSize);
mc.DeltaWldSample = results.WReadMean*(bridgeParameters.z1+bridgeParameters.z2)*pi*f*(mc.CH1Sample-mc.CH2Sample);

mc.WSample = mc.WSample + mc.DeltaWnlSample + mc.DeltaWctSample + mc.DeltaWldSample;
mc.Z2Sample = mc.Z1Sample./mc.WSample;
mc.Y2Sample = 1./mc.Z2Sample;
end