function test(imageATraiter)
% note : quelques test ave les fonction wiener2 et medfilt2 montrent que
% l'amelioration de la qualité de l'image a un impact négligeable sur la
% detection de contours avec edge.


  % capture vidéo

% vid=videoinput('winvideo',1);
% start(vid);
% im=getdata(vid,1);

 % detection contours

% im2 = rgb2gray(im);
% level = 0.5;
% im3 = im2bw(im2,level);
% img=edge(im2,'prewitt');
% img1=edge(im3, 'prewitt');
% 
% figure, imshow(im2);
% figure, imshow(img);
% figure, imshow(im);
% figure, imshow(im3);
% figure, imshow(img1);



 J  = imageATraiter;
  rotJ = imrotate(J,33,'crop');
 I = rgb2gray(J);
 rotI = imrotate(I,33,'crop');
 img = edge(rotI,'canny');

[h,t,r] =hough(img,'RhoResolution',0.5,'Theta',-90:0.5:89.5);
p = houghpeaks(h,10,'threshold',ceil(0.01*max(h(:))));
% find lines and plot
lines = houghlines(img,t,r,p,'FillGap',5,'minlength',50);
figure, imshow(rotJ), hold on
for k= 1:length(lines)
    xy = [lines(k).point1; lines(k).point2];
    plot(xy(:,1),xy(:,2),'LineWidth',2,'Color','green');
end

% detection des billes avec houghcircle
rad = 9;
[y0detect,x0detect,Accumulator] = houghcircle(img,rad,28);
plot(x0detect(:),y0detect(:),'x','LineWidth',2,'Color','blue');



