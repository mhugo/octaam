function [Y,Psi]=align_2d_set(X)
% input arg:
%  X : vertices *2 x nb_samples shape matrix
  
  nbSamples = size(X,2);
  nbVertices = size(X,1) / 2;

  %
  % 1 - Translate each sample to the origin
  %

  XX=X(1:nbVertices,:);
  YY=X(nbVertices+1:2*nbVertices,:);

  for i=1:nbSamples
    meanX=mean(XX(:,i)')';
    meanY=mean(YY(:,i)')';
    XX(:,i) = XX(:,i) - meanX;
    YY(:,i) = YY(:,i) - meanY;

  endfor;

  %
  % 2 - Choose on sample and scale it to 1
  %
  
  v=vec([XX(:,1),YY(:,1)]);
  Mn=sqrt(v'*v);
%  currentMeanX = XX(:,1) / norm(XX(:,1));
%  currentMeanY = YY(:,1) / norm(YY(:,1));
  currentMeanX = XX(:,1) / Mn;
  currentMeanY = YY(:,1) / Mn;
  currentMean = [currentMeanX, currentMeanY];


  %
  % 3 - Record the first estimate as x0
  %

  x0 = currentMean;

  epsilon = 0.001;
  iter = 0;
  maxiter = 10;
  
  while 1==1
    %
    % 4 - Align all the shapes with the current estimate
    %
    
%    gcClear;
    for i=1:nbSamples
      xi=[XX(:,i), YY(:,i)];
      
      [tx,ty,a,b] = similarity_align(xi,currentMean);
      nx = xi * [a -b; b a]' + ones(nbVertices,1) * [tx ty];
      
      XX(:,i) = nx(:,1);
      YY(:,i) = nx(:,2);

%      displayPoints2(XX(:,i), YY(:,i));
      
    endfor;
%    gcRefresh;
%    pause;

    
    %
    % 5 - Re-estimate mean from the aligned shapes
    %
    
    meanX = mean(XX')';
    meanY = mean(YY')';
    
    %
    % 6 - align with x0 and scale to 1
    %
    
    x=[meanX,meanY];
    [tx,ty,a,b] = similarity_align(x,x0);
    nx = x * [a b; -b a]' + ones(nbVertices,1) * [tx ty];
%    nx = (x  + ones(nbVertices,1) * [tx ty]) * [a b; -b a]';
    
    v=vec(nx);
    n=sqrt(v'*v);
%    nx(:,1) = nx(:,1) / norm(nx(:,1));
%    nx(:,2) = nx(:,2) / norm(nx(:,2));
    nx(:,1) = nx(:,1) / n;
    nx(:,2) = nx(:,2) / n;
    

    %
    % 7 - convergence test
    %
    
    D=(vec(x0) - vec(nx));
    D=D'*D;

    iter = iter + 1;

    if D < epsilon
      break;
    end
    
    if iter > maxiter
      break;
    end
    printf("\r\tAligning shapes ... \t[%d/%d]",iter,maxiter);

    currentMean = nx;

%    plot(XX,YY,"o");


  end


  Y=[XX;YY];
  Psi = currentMean;
end
