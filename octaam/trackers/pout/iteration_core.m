1;

n=n_geom+n_modes;

dp = zeros(n,1);
for i=1:n
  dp(i) = sum(vec(sdi(:,:,i) .* error));
end;

  
delta = inv_h * dp;
  
delta_q = delta(1:n_geom);
delta_p = delta(n_geom+1:n_geom+n_modes);

  
new_l=current_l;
for i=1:n_tex_modes
  new_l(i) = sum(vec(reshape(tex_space.v(:,i), y_res, x_res) .* error));
end;
new_l = current_l + new_l;