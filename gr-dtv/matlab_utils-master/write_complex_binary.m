%
% Copyright 2001 Free Software Foundation, Inc.
% 
% This file is part of GNU Radio
% 
% GNU Radio is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% GNU Radio is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with GNU Radio; see the file COPYING.  If not, write to
% the Free Software Foundation, Inc., 51 Franklin Street,
% Boston, MA 02110-1301, USA.
% 

function ret = write_complex_binary (data, filename)

  % usage: write_complex_binary (data, filename)
  %
  %  open filename and write the contents of a complex column vector 
  %  32 bit complex number
  %

  m = nargchk (2,2,nargin);
  if (m)
    usage (m);
  end

  f = fopen (filename, 'wb');
  
  if (f < 0)
    ret = -1;
  else
    I = real(data);
    Q = imag(data);
    fwrite (f, [I Q].', 'float');
    ret = fclose (f);
  end