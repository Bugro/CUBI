function fournirImage(imageparsec);
vid = videoinput('winvideo',1,'YUY2_160x120'); %On crée l'objet vidéo, le troisième paramêtre est le format de l'image

set(vid,'TriggerRepeat',1); % permet une acquisition d'image en continu

%imageparsec = 10;

tim = timer('TimerFcn', {@traitement,vid},'Period',1/imageparsec,'ExecutionMode','fixedRate','BusyMode','drop'); %exécute traitement toute les 1/imageparsec secondes

start(vid);

start(tim);

stop(tim);
delete(tim)
stop(vid);
delete(vid);
clear functions;
end

function traitement(obj,event,vid); %Je ne comprends pas pourquoi il faut 3 arguments ici mais il n'y a que comme ça que ça marche
    dat = getdata(vid,1); % stock une image
    test(dat(:,:,:,1)); % applique la fonction test à la première image stockée
end