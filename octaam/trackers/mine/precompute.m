printf("Precomputing Q ...\n");
if exist("Q", "var")
  printf("[MEM]\n");
else
  ax = find(vec(tmap));
  r = length(ax);%size(tex_space.v,1);
  
  T = tex_space.v(ax,:);

  Q = eye(r) - T * T';
%  Q = tex_space.v(ax,:) * tex_space.v(ax,:)';
  printf("...\n");
  Q = Q'*Q;
  %Q=Q1;
  %Q = eye(r);
  printf("[OK]\n");
end

size(Q)

printf("\tPrecomputing SDI ...\t");
if exist("sdi", "var")
  printf("[MEM]\n");
else
  sdi=zeros(y_res,x_res,n_modes+n_geom);
  
  for i=1:n_geom+n_modes
    A=(GX .* jac_x(:,:,i) + GY .* jac_y(:,:,i));
    
    sdi(:,:,i) = A;
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
    for j=i:n
      hessian(i,j)=vec(sdi(:,:,i))(ax)' * Q * vec(sdi(:,:,j))(ax);
      hessian(j,i) = hessian(i,j);
    end
  end
  inv_h=inv(hessian);
  printf("[OK]\n");
end

save(t_cache_file, "sdi", "hessian", "inv_h", "Q", "ax");