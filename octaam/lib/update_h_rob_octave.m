function H=update_h_rob_octave(sdi, outlier_map, n, hessian)
  n_total = size(sdi, 3);
# upper triangle
  for i=1:n
    for j=i:n_total
      hessian(i,j) = vec(outlier_map .* sdi(:,:,i))' * vec(sdi(:,:,j));
      hessian(j,i) = hessian(i,j);
    end;
  end;
  
  H=hessian;
end
