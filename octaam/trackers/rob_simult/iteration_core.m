ax = find(vec(tmap));
e_p=vec(error)(ax);

out_m = compute_map(e_p);

outlier_map = zeros(y_res* x_res,1);
outlier_map(ax) = out_m;
outlier_map = reshape(outlier_map, y_res, x_res);


imagesc(outlier_map);

printf("Updating SDI and Hessian ...\t");


AX=GX;
AY=GY;
for j=1:n_tex_modes
  AX = AX + current_l(j) .* nabla_a(:,:,1,j);
  AY = AY + current_l(j) .* nabla_a(:,:,2,j);
end

for i=1:n_geom+n_modes
  sdi(:,:,i) = (AX .* jac_x(:,:,i) + AY .* jac_y(:,:,i));
end


%
% Hessian
%

hessian = update_h_rob(sdi, outlier_map, n_geom + n_modes, hessian);

inv_h = inv(hessian);

printf("[OK]\n");

dp = zeros(n_total,1);
for i=1:n_total
  dp(i) = vec(outlier_map .* sdi(:,:,i))(ax)' * vec(error)(ax);
end

delta = inv_h * dp;

delta_q = delta(1:n_geom);
delta_p = delta(n_geom+1:n_geom+n_modes);
delta_lambda=delta(n_geom+n_modes+1:end);

if size(current_l,1) > 0
  new_l = current_l + delta_lambda;
else
  new_l = [];
end
