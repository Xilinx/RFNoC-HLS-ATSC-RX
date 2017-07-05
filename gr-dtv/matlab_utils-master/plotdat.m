%
%
%  Plot a complex vector versus its index (default is I vs Q)

function handle = plotdat(vec,plotIm)

if (nargin<2)
    plotIm = false;
end

handle = figure;

if (plotIm)
    plot(1:length(vec),imag(vec));
else
    plot(1:length(vec),real(vec));
end

end