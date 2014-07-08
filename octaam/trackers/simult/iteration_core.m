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

hessian = update_h(sdi, n_geom + n_modes, hessian);


inv_h = inv(hessian);

printf("[OK]\n");

dp = zeros(n_total,1);
for i=1:n_total
  dp(i) = vec(sdi(:,:,i))' * vec(error);
end;
%dp

delta = inv_h * dp;


delta_q = delta(1:n_geom);
delta_p = delta(n_geom+1:n_geom+n_modes);
delta_lambda=delta(n_geom+n_modes+1:end);


new_l = current_l + delta_lambda;
