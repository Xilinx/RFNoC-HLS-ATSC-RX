%
%
%   Adam Gannon  -  2014

function v = write_byte_binary (data, filename)



  % usage: write_byte_binary (data, filename)
  %
  %  open filename and write data to it as 8 bit unsigned ints
  %


  f = fopen (filename, 'wb');
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data);
    fclose (f);
  end
end

