%
%
%   Adam Gannon  -  2014

function v = write_short_binary (data, filename)



  % usage: write_short_binary (data, filename)
  %
  %  open filename and write data to it as signed short integers
  %


  f = fopen (filename, 'wb');
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, 'short');
    fclose (f);
  end
end

