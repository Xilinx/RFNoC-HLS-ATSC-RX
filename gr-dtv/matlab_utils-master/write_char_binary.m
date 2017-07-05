%
%
%   Adam Gannon  -  2014

function v = write_char_binary (data, filename)



  % usage: write_char_binary (data, filename)
  %
  %  open filename and write data to it as 8 bit chars
  %


  f = fopen (filename, 'wb');
  if (f < 0)
    v = 0;
  else
    v = fwrite (f, data, 'char');
    fclose (f);
  end
end

