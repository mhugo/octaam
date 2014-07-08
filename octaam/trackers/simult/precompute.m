sdi=zeros(y_res,x_res,n_total);
for i=1:n_tex_modes
  sdi(:,:,n_geom+n_modes+i) = reshape(tex_space.v(:,i),y_res,x_res);
end;

%
% precompute a part of the hessian
%

hessian=zeros(n_total,n_total);
for i=1:n_tex_modes
  for j=1:n_tex_modes
    hessian(n_geom+n_modes+i,n_modes+n_geom+j) = sum(sum(sdi(:,:,n_geom+n_modes+i) .* sdi(:,:,n_geom+n_modes+j)));
  end;

end;

save(t_cache_file, "sdi", "hessian");



