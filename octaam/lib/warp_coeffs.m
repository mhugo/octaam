%
% Returns the affine transformation parameters
% which transforms s in s0 for each triangle
%
function ST = warp_coeffs(s0, s, T)
  nbTri = size(T,1);
  ST=zeros(nbTri,6);

  for t=1:nbTri

    v0x = s0(T(t,1),1);
    v0y = s0(T(t,1),2);
    v1x = s0(T(t,2),1);
    v1y = s0(T(t,2),2);
    v2x = s0(T(t,3),1);
    v2y = s0(T(t,3),2);

    w0x = s(T(t,1),1);
    w0y = s(T(t,1),2);
    w1x = s(T(t,2),1);
    w1y = s(T(t,2),2);
    w2x = s(T(t,3),1);
    w2y = s(T(t,3),2);


    denum=(v1x - v0x) * (v2y - v0y) - (v1y - v0y) * (v2x - v0x);

    a=zeros(1,6);
    a(1) = w0x + ((v0y*(v2x - v0x) - v0x*(v2y - v0y)) * (w1x - w0x) + (v0x * (v1y - v0y) - v0y*(v1x - v0x)) * (w2x - w0x)) / denum;
    a(2) = ((v2y - v0y) * (w1x - w0x) - (v1y - v0y) * (w2x - w0x)) / denum;
    a(3) = ((v1x - v0x) * (w2x - w0x) - (v2x - v0x) * (w1x - w0x)) / denum;

    a(4) = w0y + ((v0y*(v2x - v0x) - v0x*(v2y - v0y)) * (w1y - w0y) + (v0x * (v1y - v0y) - v0y*(v1x - v0x)) * (w2y - w0y)) / denum;
    a(5) = ((v2y - v0y) * (w1y - w0y) - (v1y - v0y) * (w2y - w0y)) / denum;
    a(6) = ((v1x - v0x) * (w2y - w0y) - (v2x - v0x) * (w1y - w0y)) / denum;

    ST(t,:) = a;
  end
end
