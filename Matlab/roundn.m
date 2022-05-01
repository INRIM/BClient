function y = roundn(x,varargin)
global isOctave
if isOctave
    if nargin == 1
        n = 0;
    else
        n = varargin{1};
    end
    if nargin <= 2 
        y = round(x*10^n)/10^n;
    else
        switch varargin{2}
            case 'decimals'
                y = round(x*10^n)/10^n;
            case 'significant'
                m = ceil(log10(abs(x)));
                y = round(x*10^(n-m))/10^(n-m);
            otherwise
                error('Third input must be either ''decimals'' or ''significant''');
        end
    end
else
    y = round(x,varargin{:});
end
end