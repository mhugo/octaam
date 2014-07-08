function R=get_tmap(vertices, triangles, r_size)
  %%%
  %%% vertices is 0-based
  %%%
  R=zeros(r_size(2), r_size(1));
  for i=1:size(triangles,1)
    X=vertices(triangles(i,:),1);
    Y=vertices(triangles(i,:),2);

    PP=poly2mask(X,Y,r_size(2), r_size(1)) .* i;

    R=max(R,PP);

  end
end
