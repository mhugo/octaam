function [tx,ty,a,b]=similarity_align(V1,V2)
% size(V1) = size(V2) = [n, 2]

%  V1(:,1) = V1(:,1) - mean(V1(:,1));
%  V1(:,2) = V1(:,2) - mean(V1(:,2));
%  V2(:,1) = V2(:,1) - mean(V2(:,1));
%  V2(:,2) = V2(:,2) - mean(V2(:,2));
%sum(vec(V2))

  n = size(V1,1);
  Sxx = V1(:,1)'*V1(:,1) / n;
  Syy = V1(:,2)'*V1(:,2) / n;
  Sxxp = V1(:,1)'*V2(:,1) / n;
  Syyp = V1(:,2)'*V2(:,2) / n;
  Sxyp = V1(:,1)'*V2(:,2) / n;
  Syxp = V1(:,2)'*V2(:,1) / n;

  tx = sum(V2(:,1)) / n;
  ty = sum(V2(:,2)) / n;

  a = (Sxxp + Syyp) / (Sxx+Syy);
  b = (Sxyp - Syxp) / (Sxx+Syy);
endfunction;
