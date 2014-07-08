1;

size(Q)

n=n_geom+n_modes;

error = vec(proj) - tex_space.psi;


dp = zeros(n,1);
for i=1:n
  dp(i) = vec(sdi(:,:,i))(ax)' * (Q * error(ax));
end;

  
delta = inv_h * dp;
  
delta_q = delta(1:n_geom);
delta_p = delta(n_geom+1:n_geom+n_modes);

  
new_l = tex_space.v' * (vec(proj) - tex_space.psi);