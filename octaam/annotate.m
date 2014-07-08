load 35_coart.set;

source("lib/include.m");

for i=1:length(data.Models)
  imfile = strcat(data.ImgDir, "/", data.Models(i,:), ".", data.ImgExt);
  I=imread(imfile);
  R=double(I(:,:,1));
  G=double(I(:,:,2));
  B=double(I(:,:,3));
  X = gcPixels(R,G,B,35);

  data.shapes(:,1:2,i) = X;
end

