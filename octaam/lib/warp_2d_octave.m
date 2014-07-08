function J = warp_2d_octave(input, wcoeffs, triangles, tMap, sizem)
  width = sizem(2);
  height = sizem(1);

  J = zeros(height, width);

  w = size(input,2);
  h = size(input,1);

  M = w*h;

  nb_t = size(triangles,1);

  for t=1:nb_t
    a = wcoeffs(t,:);
    [y,x] = find(tMap == t);
    
    idx = y + (x-1) * height;

    cx = round(a(1) + a(2) .* (x-1) + a(3) .* (y-1)) + 1;
    cy = round(a(4) + a(5) .* (x-1) + a(6) .* (y-1)) + 1;

    cidx = cy + (cx-1) * h;

    % clamp pixel coordinates
    cidx = (find((cidx) > 0) && (cidx) <= M) .* cidx;

    % copy pixels
    J(idx) = input(cidx);

  end
%    imagesc(J);
%    drawnow;
%    pause;
end
