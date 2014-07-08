printf("\tPrecomputing SDI ...\t");
if exist("sdi", "var")
  printf("[MEM]\n");
else
  sdi=zeros(y_res,x_res,n_modes+n_geom);
  
  for i=1:n_geom+n_modes
    sdi(:,:,i) = GX .* jac_x(:,:,i) + GY .* jac_y(:,:,i);
  end
  printf("[OK]\n");
end

printf("\tHessian ...\t\t");
if exist("inv_h", "var")
  printf("[MEM]\n");
else
  n=n_modes+n_geom;
  hessian=zeros(n,n);
  for i=1:n
    for j=1:n
      hessian(i,j)=sum(vec(sdi(:,:,i) .* sdi(:,:,j)));
    end
  end
  inv_h=inv(hessian);
  printf("[OK]\n");
end

if exist("hessian_t", "var")
  printf("[MEM]\n");
else
  nb_t = length(triangles);
  hessian_t = zeros(n_geom + n_modes, n_geom + n_modes, nb_t);
  
  for i=1:nb_t
    printf("%d / %d\n", i, nb_t);
    for j=1:n_geom + n_modes;
      for k=1:n_geom + n_modes;
	hessian_t(j,k,i) = sum(vec(sdi(:,:,j) .* sdi(:,:,k) .* (tmap == i)));
      end
    end
  end
end

save("-binary",t_cache_file, "sdi", "hessian", "inv_h","hessian_t");
