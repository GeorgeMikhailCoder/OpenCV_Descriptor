I = imread('fig.png');
I = imadjust(I,[0 1], [1 0], 1);

imshow(I);
imsave()