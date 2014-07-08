1;

ax = find(vec(tmap));
e_p=vec(error)(ax);

out_m = compute_map(e_p);

outlier_map = zeros(y_res* x_res,1);
outlier_map(ax) = out_m;
outlier_map = reshape(outlier_map, y_res, x_res);
  
out = zeros(y_res, x_res);
for tri=1:length(triangles)
  tx = find(vec(tmap == tri));
  if length(tx) == 0
    continue;
  end
  m = mean(vec(outlier_map)(tx));
  rob_coeff(tri) = m;
  out = out + m .* (tmap == tri);
end
imagesc(out);


if n_tex_modes > 0
  
  hessian_a = zeros(n_tex_modes);
  for i=1:n_tex_modes
    for j=1:n_tex_modes
      hessian_a(i,j) = sum(vec(outlier_map) .* tex_space.v(:,i) .* tex_space.v(:,j));
    end
  end
  inv_h_a = inv(hessian_a);
  
  d_l = inv_h_a * tex_space.v' * (vec(outlier_map .* error));
  
  error_app = error - reshape(tex_space.v * d_l, y_res, x_res);
  
  new_l = current_l + d_l;
else
  new_l = [];
  error_app = error;
end

n=n_geom+n_modes;

dp = zeros(n,1);
for i=1:n
  dp(i) = vec(outlier_map .* sdi(:,:,i))(ax)' * vec(error_app)(ax);
end;

hessian = zeros(n_geom + n_modes);
for i=1:length(triangles)
  hessian = hessian + rob_coeff(i) .* hessian_t(:,:,i);
end
inv_h = inv(hessian);
  
delta = inv_h * dp;
  
delta_q = delta(1:n_geom);
delta_p = delta(n_geom+1:n_geom+n_modes);


