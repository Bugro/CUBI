function realVideo()
 
% Define frame rate
% De base il �tait � 10 mais � 5 c'est fluide et on gagne en vitesse (�
% mesurer)
% Attention cette variable est utilis�e dans les param�tres du timer!
NumberFrameDisplayPerSecond=5;
 
% Open figure
hFigure=figure(1);
 
% Set-up webcam video input
vid = videoinput('winvideo', 1);

 
% Set parameters for video
% Acquire only one frame each time
set(vid,'FramesPerTrigger',1);
% Go on forever until stopped
set(vid,'TriggerRepeat',Inf);
% Get a grayscale image
set(vid,'ReturnedColorSpace','grayscale');
% On prend moins d'image pour augmenter la r�activit�
set(vid,'FrameGrabInterval',2);
triggerconfig(vid, 'Manual');
 
% set up timer object
TimerData=timer('TimerFcn', {@FrameRateDisplay,vid},'Period',1/NumberFrameDisplayPerSecond,'ExecutionMode','fixedRate','BusyMode','drop'); %ex�cute FrameRateDisplay toute les 1/NumberFrameDisplayPerSecond secondes

% Start video and timer object
start(vid);
start(TimerData);
%get(vid); %Renvoie des infos sur vid
% We go on until the figure is closed (two times, i don't know why)
% Tant que la figure n'est pas ferm�e on continu � ex�cuter
% FrameRateDisplay quand le timer l'indique
uiwait(hFigure);
 
% Clean up everything
stop(TimerData);
delete(TimerData);
stop(vid);
delete(vid);
% clear persistent variables
clear functions;
end
 
% This function is called by the timer to display one frame of the figure
 
function FrameRateDisplay(obj, event,vid)
%On d�fini des variables persistantes 
persistent IMbefore;
trigger(vid);
while get(vid,'FramesAvailable')<1  %Wait until at least 1 frame is available
      unavailable=1;
end
IMbefore=getdata(vid,1,'uint8');%On stock l'info gr�ce � getdata
imagesc(IMbefore);
end