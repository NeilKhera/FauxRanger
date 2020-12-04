disp('Reading file...');
fileID = fopen('LDEM_85S_10M.raw');
A = fread(fileID, [30336, 30336], 'int16=>int16');

disp('File read complete');
disp(size(A));
disp('Cropping matrix...');

coord1 = 10280;
coord2 = 5140;
B = A(coord1:coord1+2275, coord2:coord2+2275);
disp('Matrix crop complete');
disp(B(100,100));

disp('Casting and rescaling...');
C = cast(B, 'single');
D = C / 2;
disp('Casting and rescaling complete');
disp(D(100,100));

disp('Saving new file...');
outputID = fopen('out.raw', 'w');
fwrite(outputID, D, 'single');
fclose(fileID);
fclose(outputID);

disp('Done!');