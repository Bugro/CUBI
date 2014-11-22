vid = videoinput('winvideo',1,'YUY2_160x120'); %On cr�e l'objet vid�o, le troisi�me param�tre est le format de l'image

set(vid,'TriggerRepeat',Inf); % permet une acquisition d'image en continu

start(vid);


dat = getdata(vid,3); % stock une image
for i = 1:3
    test(dat(:,:,:,i)); % applique la fonction test � la premi�re image stock�e
end
stop(vid);