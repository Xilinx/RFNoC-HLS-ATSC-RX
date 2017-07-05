%
%
%   Adam Gannon  -  2014

function v = write_int_binary (data, filename)



  % usage: write_int_binary (data, filename)
  %
  %  open filename and write data to it as signed integers
  %


  f = fopen (filename, 'wb');
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, 'int');
    fclose (f);
  end
end

