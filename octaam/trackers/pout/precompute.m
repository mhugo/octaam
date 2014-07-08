printf("\tPrecomputing SDI ...\t");
if exist("sdi", "var")
  printf("[MEM]\n");
else
  sdi=zeros(y_res,x_res,n_modes+n_geom);
  
  for i=1:n_geom+n_modes
    A=(GX .* jac_x(:,:,i) + GY .* jac_y(:,:,i));
    
    B=A;
    for j=1:n_tex_modes
      Aj=reshape(tex_space.v(:,j), y_res, x_res);
      B=B-sum(vec(Aj .* A)) .* Aj;
    end
    sdi(:,:,i) = B;
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
inv_h_diag = 1./diag(hessian);

save(t_cache_file, "sdi", "hessian", "inv_h", "inv_h_diag");
