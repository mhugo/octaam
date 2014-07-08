function I = imread2(fname)
  % behavior depends on Octave version
  % starting from 2.1.73, imread() returns a 3-dimension array
  % else, it returns 3 matrices

  v = version;
  if !ischar(v) &  str2double(v(3:end)) < 1.73
    [R,G,B] = imread(fname);
    I = zeros(size(R,1), size(R,2), 3);
    I(:,:,1) = R;
    I(:,:,2) = G;
    I(:,:,3) = B;
  else
    I = imread(fname);
  end
end
