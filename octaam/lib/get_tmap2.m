% get tmap function ok for big pictures
function R=get_tmap2(vertices, triangles, r_size)
  %%%
  %%% vertices is 0-based
  %%%
  R=zeros(r_size(2), r_size(1));

  X=vertices(:,1);
  Y=vertices(:,2);
  rx = [floor(min(X)), ceil(max(X))];
  ry = [floor(min(Y)), ceil(max(Y))];
  w = rx(2)-rx(1)+1
  h = ry(2)-ry(1)+1

  

  PP=zeros(h, w);
  for i=1:size(triangles,1)
    X=vertices(triangles(i,:),1);
    Y=vertices(triangles(i,:),2);

%    mx = min(X)
%    Mx = max(X)

    % avoid little bug
    my = floor(min(Y));
    My = floor(max(Y));
    if abs(my - My) <= 1
      continue;
    end

    P=poly2mask(X-rx(1),Y-ry(1),h,w) .* i;
    PP=max(PP,P);
  end
  R((ry(1):ry(2))+1,(rx(1):rx(2))+1) = PP;
end