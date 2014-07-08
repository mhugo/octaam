function U=rgb2gray(R,G,B)
U = round(R .* 0.3 + G .* 0.6 + B .* 0.1);
end