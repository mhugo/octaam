% compute PCA on the data X
% nb is the number of dimension to retain
% if nb > 1, it's a number
% if nb < 1, it's a variance percentage
function [V,L,nb,vart]=compute_pca(X,Psi,nb)

  nbSamples = size(X,2);

  C=X - Psi * ones(1, nbSamples);
%  for i=1:nbSamples
%    C(:,i)=C(:,i)-Psi;
%  endfor;

  %
  % Pseudo-covariance
  %
  


%  [U,S,V] = svd(C);
%  L = (S'*S) ./ (nbSamples);
%  V=-V;

  M=C'*C;

  [V, L] = eig(M);

  L=diag(L);
  Vt=sum(L);

  [S,Idx] = sort(L,'descend');


%  L=flipud(L);
%  V=fliplr(V);
  L=L(Idx);
  V=V(:,Idx);


  if nb >= 1
%    V=V(:,nbSamples-nb+1:nbSamples);
    L=L(1:nb);
    vart=sum(L) / Vt;
  else
    vart = nb;
    for i=1:nbSamples
      if sum(L(1:i)) > (vart * Vt)
	nb=i;
	vart = sum(L(1:i)) / Vt;
	break;
      end
    end
    L=L(1:nb);
  end

  %
  % Real eigen-vectors
  %

  V=C*V;


  %
  % normalize
  %
  
  V=V(:,1:nb);
  for i=1:nb
    V(:,i) = V(:,i) / norm(V(:,i));
  end

end
