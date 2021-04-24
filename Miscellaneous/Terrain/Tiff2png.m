[A, R] = readgeoraster("Haworth_final_adj_5mpp_surf.tif");
m1 = max(double(A), [], 'all');
m2 = min(double(A), [], 'all');
geotiffinfo("Haworth_final_adj_5mpp_surf.tif")
%S = arrayfun(@(x) ((x - m2) / (m1 - m2)) * 65535, A);
(m1 - m2) / 256
%imwrite(uint16(S), 'out.png');