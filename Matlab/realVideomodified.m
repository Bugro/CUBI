function realVideo()
 
% Define frame rate
% De base il était à 10 mais à 5 c'est fluide et on gagne en vitesse (à
% mesurer)
% Attention cette variable est utilisée dans les paramêtres du timer!
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
% On prend moins d'image pour augmenter la réactivité
set(vid,'FrameGrabInterval',2);
triggerconfig(vid, 'Manual');
 
% set up timer object
TimerData=timer('TimerFcn', {@FrameRateDisplay,vid},'Period',1/NumberFrameDisplayPerSecond,'ExecutionMode','fixedRate','BusyMode','drop'); %exécute FrameRateDisplay toute les 1/NumberFrameDisplayPerSecond secondes

% Start video and timer object
start(vid);
start(TimerData);
%get(vid); %Renvoie des infos sur vid
% We go on until the figure is closed (two times, i don't know why)
% Tant que la figure n'est pas fermée on continu à exécuter
% FrameRateDisplay quand le timer l'indique
uiwait;
 
% Clean up everything
stop(TimerData);
delete(TimerData);
stop(vid);
delete(vid);
% clear persistent variables
clear functions;
 
% This function is called by the timer to display one frame of the figure
 
function FrameRateDisplay(obj, event,vid)
%On défini des variables persistantes 
persistent IMbefore;
persistent IMafter;
persistent handlesRaw;
persistent handlesPlot;
trigger(vid);
IMbefore=getdata(vid,1,'uint8');%On stock l'info grâce à getdata
%IMafter=test(IMbefore(:,:,:,1)); 

if isempty(handlesRaw)
   % if first execution, we create the figure objects
   subplot(2,1,1);
   handlesRaw=imagesc(IMbefore);
   title('CurrentImage');
 
   % Plot first value
   %Values=mean(IM(:));
   %subplot(2,1,2);
   %handlesPlot = imagesc(IMafter);
   %handlesPlot=plot(Values);
   %title('Average of Frame');
   %xlabel('Frame number');
   %ylabel('Average value (au)');
else
   % We only update what is needed
   set(handlesRaw,'CData',IMbefore); %met à jour handlesRaw
   %Value=mean(IM(:));
   %OldValues=get(handlesPlot,'YData');
   %set(handlesPlot,'CData',IMafter); %met à jour handlesPlot
end