disp('Reading file...');
fileID = fopen('LDEM_875S_5M.raw');
A = fread(fileID, [30336, 30336], 'int16=>int16');

disp('File read complete');
disp(size(A));
disp('Cropping matrix...');

coord1 = 15169;
coord2 = 15168 - 5999;
B = A(coord1:coord1+4032, coord2:coord2+4032);
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