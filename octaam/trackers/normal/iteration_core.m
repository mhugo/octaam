1;

%error = error .* outlier_map;

if n_tex_modes > 0
  d_l = tex_space.v' * vec(error);

  error = error - reshape(tex_space.v * d_l, y_res, x_res);
  new_l = current_l + d_l; 
else
  new_l = zeros(0,1);
end

n=n_geom+n_modes;

dp = zeros(n,1);
for i=1:n
  dp(i) = vec(sdi(:,:,i))' * vec(error);
end;

  
delta = inv_h * dp;
  
delta_q = delta(1:n_geom);
delta_p = delta(n_geom+1:n_geom+n_modes);

