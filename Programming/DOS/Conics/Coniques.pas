{                             ����������������������������������Ŀ
                              � Auteur : COURTIAT Cyril  CSII-1A �
                              ������������������������������������
}

type
          ZPolyPoints= array[1..4] of record
                                       xe,
                                       ye,
                                       ze: real;
                                      end;

type
        _attrib= record        { attributs des objets coniques. }
              _a,
              _b,
              _c,
              _k,
              _xo,
              _yo,             { coordonn�es de l'origine. }
              _zo,
              _xs,
              _ys,             { coordonn�es du centre d'homot�thie. }
              _zs,
              _h: real;        { coefficient d'homot�thie. }
              _pas: byte;      { le pas entre chaque point d�termine la longueur du segment. }
        end;

        point = ^chaine;       { liste cha�n�e, par xlien, des points qui d�terminent }
        chaine= record         { un objet en m�moire. }
                info : byte;   { info = [_move,_line,_stop]. }
                xreal,
                yreal,
                zreal: real;
                xlien,
                ylien: point;  { un point est cha�n�, par ylien, avec son pr�d�cesseur }
        end;                   { situ� sur la m�me abscisse. }

const
        _langues = 2;
        _francais= 1;
        _anglais = 2;
        _allemand= 3;

        SVGA  = 255;
        SVGALo  = 1;
        SVGAMed = 3;
        SVGAHi  = 4;

        _esc  = #27;
        _enter= #13;
        _left = #75;
        _right= #77;
        _up   = #72;
        _down = #80;
        _home = #71;
        _end  = #79;
        _f1   = #59;
        _f2   = #60;
        _f3   = #61;
        _f4   = #62;
        _f5   = #63;
        _f6   = #64;
        _f7   = #65;
        _f8   = #66;
        _f9   = #67;
        _f0   = #68;

        _move = 01;   { d�but de la cha�ne et de chaque premi�re abscisse.}
        _line = 02;   { ce point et son pr�d�cesseur (ou successeur) constituent un segment. }
        _stop = 03;   { fin de la cha�ne. }

        _kshift = 4;
        _borne  = 70;          { -borne et +borne sont les intervalles sur les axes X & Y.}
        _pas_min= 2;           { le pas minimum saisi. }
        _volumes= 6;           { le nombre de coniques (ou volumes) m�moris�es. }
        _couleur: array[1.._volumes] of TColor= (clBlue,clRed,clMaroon,clGreen,clNavy,clFuchsia);
        _volume : array[1.._volumes] of _attrib=
        (
        (_a:+1   ;_b:+1  ;_c:-1  ;_k:+    0;_xo:+0;_yo:+0;_zo:0;_xs:0;_ys:0;_zs:0;_h:+1;_pas:8),
        (_a:-3   ;_b:+3  ;_c:-3  ;_k:-17000;_xo:+0;_yo:+0;_zo:0;_xs:0;_ys:0;_zs:0;_h:+1;_pas:8),
        (_a:+0.01;_b:+0.7;_c:+0.2;_k:+ 3000;_xo:+0;_yo:+0;_zo:0;_xs:0;_ys:0;_zs:0;_h:+1;_pas:8),
        (_a:+2   ;_b:+0  ;_c:+1  ;_k:+ 5000;_xo:+0;_yo:+0;_zo:0;_xs:0;_ys:0;_zs:0;_h:+1;_pas:5),
        (_a:-0.5 ;_b:+0  ;_c:0.75;_k:+  500;_xo:+0;_yo:+0;_zo:0;_xs:0;_ys:0;_zs:0;_h:+1;_pas:8),
        (_a:+0   ;_b:+0  ;_c:+1  ;_k:+ 3000;_xo:+0;_yo:+0;_zo:0;_xs:0;_ys:0;_zs:0;_h:+1;_pas:28)
        );
                               { attributs de coniques pr�d�finis. }

const    _conic  : array[1..6,1.._langues] of string[26]= (
                   ('ellipso�de                ','ellipsoid                 '),
                   ('hyperbolo�de � une nappe  ','hyperboloid of one sheet  '),
                   ('hyperbolo�de � deux nappes','hyperboloid of two sheets '),
                   ('cylindre elliptique       ','elliptical cylinder       '),
                   ('cylindre hyperbolique     ','hyperbolic cylinder       '),
                   ('deux plans parall�les     ','two parallel plans        '));

const   _conique: array[1.._langues] of string[8]=('Conique ','Conic   ');
        _origine: array[1.._langues] of string[9]=('  Origine','  Origin ');
        _homo   : array[1.._langues] of string[11]=('homoth�ties','homothetia ');
        _special: array[1.._langues] of string[9]=('  Sp�cial','  Special');
        _graph  : array[1.._langues] of string[10]=(' Graphique','  Graphic');
        _fin    : array[1.._langues] of string[4]=(' fin',' end');

var       depth,xshift,yshift,zshift: integer;
          a,b,c,d,e,f,g,h,i: real;

var
        xangle,                                      { angles de rotation autour des axes.}
        yangle,
        zangle      : integer;
        mo          : array[1.._volumes] of point;   { pointeur de d�but de cha�ne. }
        nom         : array[1.._volumes] of byte;    { identit� de chaque conique. }
        xnom        : byte;                          { var temporaire pour saisir l'identit�. }
        volume      : array[1.._volumes] of _attrib; { attributs variables de chaque coniques.}
        id          : byte;                          { num�ro conique en cours d'utilisation. }
        segments    : array[1.._volumes] of word;    { nombre de segments par conique. }
        memtotale   : longint;                       { taille du tas � l'initialisation. }
        choix       : char;
        buffer      : string[10];                    { buffer de saisie alphanum�rique. }
        buffer1     : array[0..2] of char;           { tampons assign�s au fichier d'entr�e }
        buffer2     : array[0..7] of char;           { standard INPUT. bidouille utile pour }
        buffer3     : array[0..4] of char;           { limiter la saisie avec READ[LN]. }
        buffer4     : array[0..6] of char;
        fontsize,
        chif1       : byte;                          { variables temporaires pour le test de }
        chif6       : real;                          { saisie num�rique. }
        chif3,
        testnum,                                     { test de num�ricit�. }
        GraphPilote,
        GraphMode,
        SvgaMode,
        GraphErreur : integer;
        BGIpath     : string;
        orgmode     : word;
        langue      : byte;



function GetMaxX: integer;
begin
     GetMaxX := FormConic.width;
end;

function GetMaxY: integer;
begin
     GetMaxY := FormConic.height;
end;

{ calcule des param�tres de rotation. }
procedure Trigo(xa,ya,za: integer);

          function rad(degre: integer): real;
          begin
               rad:=(pi*degre)/180;
          end;
begin
     a:= cos(rad(za))*cos(rad(ya));
     b:= sin(rad(za))*cos(rad(ya));
     c:=-sin(rad(ya));
     d:=-sin(rad(za))*cos(rad(xa))+cos(rad(za))*sin(rad(ya))*sin(rad(xa));
     e:= cos(rad(za))*cos(rad(xa))+sin(rad(za))*sin(rad(ya))*sin(rad(xa));
     f:= cos(rad(ya))*sin(rad(xa));
     g:= sin(rad(za))*sin(rad(xa))+cos(rad(za))*sin(rad(ya))*cos(rad(xa));
     h:=-cos(rad(za))*sin(rad(xa))+sin(rad(za))*sin(rad(ya))*cos(rad(xa));
     i:= cos(rad(ya))*cos(rad(xa));
end;

{ positionne le curseur graphique en 3D. }
procedure Zmove(xe,ye,ze: real);
var       q,xproj,yproj,xrot,yrot,zrot: real;
begin
     xrot:= a*xe+d*ye+g*ze+xshift; {calcule des rotations et des translations. }
     yrot:= b*xe+e*ye+h*ze+yshift;
     zrot:= c*xe+f*ye+i*ze+zshift;
     q:= 1-(zrot/depth);           { projection centrale de profondeur depth.}
     xproj:= xrot/q + GetMaxX /2;  { transformation 3D -> 2D. }
     yproj:= yrot/q + GetMaxY /2;
     canvas.MoveTo(trunc(xproj),trunc(yproj));
end;

{ allume un point de couleur pixel. }
procedure Zplot(xe,ye,ze: real; pixel: TColor);
var       q,xproj,yproj,xrot,yrot,zrot: real;
begin
     xrot:= a*xe+d*ye+g*ze+xshift;
     yrot:= b*xe+e*ye+h*ze+yshift;
     zrot:= c*xe+f*ye+i*ze+zshift;
     q:= 1-(zrot/depth);                         { cf Zmove. }
     xproj:= xrot/q + GetMaxX / 2;
     yproj:= yrot/q + GetMaxY / 2;
     canvas.Pixels[trunc(xproj),trunc(yproj)] := pixel;
end;

{ trace une ligne de couleur pixel. }
procedure Zline(xe1,ye1,ze1,xe2,ye2,ze2: real; pixel: TColor);
var       q, xproj1,yproj1,xproj2,yproj2,xrot1,yrot1,zrot1,xrot2,yrot2,zrot2: real;
begin
     xrot1:= a*xe1+d*ye1+g*ze1+xshift;
     yrot1:= b*xe1+e*ye1+h*ze1+yshift;
     zrot1:= c*xe1+f*ye1+i*ze1+zshift;
     xrot2:= a*xe2+d*ye2+g*ze2+xshift;
     yrot2:= b*xe2+e*ye2+h*ze2+yshift;
     zrot2:= c*xe2+f*ye2+i*ze2+zshift;
     q:= 1-(zrot1/depth);                        { cf Zmove. }
     xproj1:= xrot1/q + GetMaxX / 2;
     yproj1:= yrot1/q + GetMaxY / 2;
     q:= 1-(zrot2/depth);
     xproj2:= xrot2/q + GetMaxX / 2;
     yproj2:= yrot2/q + GetMaxY / 2;
     canvas.Pen.Color := pixel;
     canvas.MoveTo(trunc(xproj1),trunc(yproj1));
     canvas.LineTo(trunc(xproj2),trunc(yproj2));
end;

{ trace une ligne de couleur pixel. }
procedure ZlineTo(xe,ye,ze: real; pixel: TColor);
var       q,xproj,yproj,xrot,yrot,zrot: real;
begin
     xrot:= a*xe+d*ye+g*ze+xshift;
     yrot:= b*xe+e*ye+h*ze+yshift;
     zrot:= c*xe+f*ye+i*ze+zshift;
     q:= 1-(zrot/depth);                         { cf Zmove. }
     xproj:= xrot/q + GetMaxX / 2;
     yproj:= yrot/q + GetMaxY / 2;
     canvas.Pen.Color := pixel;
     canvas.LineTo(trunc(xproj),trunc(yproj)); { relativement au curseur graphique. }
end;

{ trace en 3D une boite de couleur pixel. }
procedure Zbox(xe1,ye1,ze1,xe2,ye2,ze2: real; pixel: TColor);
begin
     Zline(xe1,ye1,ze1,xe1,ye1,ze2,pixel);
     Zline(xe1,ye1,ze2,xe1,ye2,ze2,pixel);
     Zline(xe1,ye2,ze2,xe1,ye2,ze1,pixel);
     Zline(xe1,ye2,ze1,xe1,ye1,ze1,pixel);

     Zline(xe2,ye1,ze1,xe2,ye1,ze2,pixel);
     Zline(xe2,ye1,ze2,xe2,ye2,ze2,pixel);
     Zline(xe2,ye2,ze2,xe2,ye2,ze1,pixel);
     Zline(xe2,ye2,ze1,xe2,ye1,ze1,pixel);

     Zline(xe1,ye1,ze2,xe2,ye1,ze2,pixel);
     Zline(xe1,ye2,ze2,xe2,ye2,ze2,pixel);
     Zline(xe1,ye1,ze1,xe2,ye1,ze1,pixel);
     Zline(xe1,ye2,ze1,xe2,ye2,ze1,pixel);
end;

{ trace un polygone � quatre c�t�s. }
procedure ZFillPoly(sommets: ZPolyPoints);
var       q,xrot,yrot,zrot: real; proj: array[1..4] of TPoint; loop: byte;
begin
     for loop:=1 to 4 do
       with sommets[loop],proj[loop] do
         begin
              xrot:= a*xe+d*ye+g*ze+xshift;
              yrot:= b*xe+e*ye+h*ze+yshift;
              zrot:= c*xe+f*ye+i*ze+zshift;
              q:= 1-(zrot/depth);                { cf Zmove. }
              x:= trunc(xrot/q + GetMaxX / 2);
              y:= trunc(yrot/q + GetMaxY / 2);
         end;
     canvas.Polygon(proj);
end;


procedure Axes;
begin
     Zline(0,0,0,50,0,0,clBlue);  { affiche les axes X,Y & Z au centre de la zone de trac� }
     Zline(0,0,0,0,50,0,clTeal);     { graphique. }
     Zline(0,0,0,0,0,50,clGreen);
end;

procedure Omega(id_cone: byte);      { encadre le centre d'homot�thie. }
begin
     with volume[id_cone] do
       begin
            canvas.Brush.Color := clWhite;
            Zmove(_xs,_ys,_zs);
            canvas.Rectangle(canvas.PenPos.X-2,canvas.PenPos.Y-2,canvas.PenPos.X+2,canvas.PenPos.Y+2);
       end;
end;

procedure Information;
const    _titre  : array[1.._langues] of string[62]= (
                   '�                         STATISTIQUES                       �',
                   '�                          STATISTICS                        �');
         _libelle: array[1.._langues] of string[62]= (
                   '� n�        conique             segments  allocation m�moire �',
                   '� n�        conic               segments  allocation memory  �');
         _dispo  : array[1.._langues] of string[62]= (
                   '�                  m�moire disponible                        �',
                   '�                    available memory                        �');
var       cone: byte; sumpts,alloc: longint; taux: real;
          coneStr : String[8];
          segmentStr : String[8];
          allocStr : String[8];
          tauxStr : String[8];
          memAvailStr, memTotalStr : String[8];
begin
     Axes;
     sumpts:=0;
     for cone:=1 to _volumes do         { nombre totale de segments pour toutes les coniques. }
         inc(sumpts,segments[cone]);
     {TextColor(lightblue);}
     canvas.Brush.Color := clBlue;
     {GotoXY(18,27-_volumes);
     canvas.TextOut(100,50-_volumes,'������������������������������������������������������������Ŀ');
     {GotoXY(18,28-_volumes);
     canvas.TextOut(100,canvas.PenPos.Y-_volumes,_titre[langue]);
     {GotoXY(18,29-_volumes);
     canvas.TextOut(100,canvas.PenPos.Y-_volumes,'������������������������������������������������������������Ĵ');
     {GotoXY(18,30-_volumes);
     canvas.TextOut(100,canvas.PenPos.Y-_volumes,_libelle[langue]); }
     for cone:=1 to _volumes do
      begin
        alloc:=longint(segments[cone])*SizeOf(chaine);    { allocation m�moire d'une conique. }
        taux :=(segments[cone]/sumpts)*100;       { taux d'occupation par rapport aux autres. }
        {GotoXY(18,30-_volumes+cone);
        Write('� ',cone:1,' ',_conic[nom[cone],langue],'   ',segments[cone]*2:5,'     ',
               alloc:6,' B  ',taux:4:1,' %  �');}
        str(cone:1,coneStr);
        str(segments[cone]*2:5,segmentStr);
        str(alloc:6,allocStr);
        str(taux:4:1,tauxStr);
        FormConic.ListBoxInfo.Items.Add( coneStr + ' ' + _conic[nom[cone],langue] + '   ' + segmentStr + '     '
                                         + allocStr + ' B  ' + tauxStr + ' %');
      end;
     {GotoXY(18,31);
     canvas.TextOut(100,canvas.PenPos.Y,'������������������������������������������������������������Ĵ');
     {GotoXY(18,32);
     canvas.TextOut(100,canvas.PenPos.Y,_dispo[langue]);
     {GotoXY(18,33);
     canvas.TextOut(100,canvas.PenPos.Y,'��������������������������������������������������������������');
     {GotoXY(58,32);
     str(MemAvail:6,memAvailStr);
     str(memtotale:6,memTotalStr);
     canvas.TextOut(150,canvas.PenPos.Y, memAvailStr + ' B / ' + memTotalStr + ' B'); }
     Axes;
end;

procedure Trace1(id_cone: byte); { affiche une conique avec une structure en lignes crois�es. }
var       xreal1,yreal1,zreal11,zreal12,
          xreal2,yreal2,zreal21,zreal22,
          xrealh,yrealh,zrealh1,zrealh2: real;
          mo1,mo2: point;
begin
     Axes;
     {Horloge(74,31*fontsize,yellow,lightcyan);}
     canvas.Pen.Mode := pmCopy;

     mo1:=mo[id_cone];
     while not (mo1^.info=_stop) do
       with volume[id_cone] do
         begin
            xreal2 :=_h*( mo1^.xreal+_xo)+(1-_h)*_xs;
            yreal2 :=_h*( mo1^.yreal+_yo)+(1-_h)*_ys;
            zreal21:=_h*( mo1^.zreal+_zo)+(1-_h)*_zs;
            zreal22:=_h*(-mo1^.zreal+_zo)+(1-_h)*_zs;
            if mo1^.info=_line then
               begin
                    Zline(xreal1,yreal1,zreal11,xreal2,yreal2,zreal21,_couleur[id_cone]);
                    Zline(xreal1,yreal1,zreal12,xreal2,yreal2,zreal22,_couleur[id_cone]);
               end;
            mo2:=mo1^.ylien;     { le point qui pr�c�de sur la m�me abscisse }
            if not (mo2=NIL) then
               begin     { on trace sur Y le segment (et son sym�trique en Z). }
                    xrealh :=_h*( mo2^.xreal+_xo)+(1-_h)*_xs;
                    yrealh :=_h*( mo2^.yreal+_yo)+(1-_h)*_ys;
                    zrealh1:=_h*( mo2^.zreal+_zo)+(1-_h)*_zs;
                    zrealh2:=_h*(-mo2^.zreal+_zo)+(1-_h)*_zs;
                    Zline(xreal2,yreal2,zreal21,xrealh,yrealh,zrealh1,_couleur[id_cone]);
                    Zline(xreal2,yreal2,zreal22,xrealh,yrealh,zrealh2,_couleur[id_cone]);
               end;
            xreal1 :=xreal2;     { le point en cours devient le dernier point }
            yreal1 :=yreal2;     { pour un tracer de ligne relatif. }
            zreal11:=zreal21;
            zreal12:=zreal22;
            mo1:=mo1^.xlien;
       end;

     Omega(id_cone);
     {Horloge(74,31*fontsize,black,black); }
     canvas.Pen.Mode := pmXor;
     Axes;
end;

procedure Trace2(id_cone: byte);       { affiche une conique avec une structure en polygones. }
var       sommets: ZPolyPoints; mo1: point;
begin
     Axes;
     { Horloge(74,31*fontsize,yellow,lightcyan); }
     canvas.Pen.Mode := pmCopy;
     canvas.Pen.Color := _couleur[id_cone];
     canvas.Pen.Style := psSolid;

     mo1:=mo[id_cone];
     while not (mo1^.info=_stop) do
       begin
           if (mo1^.xlien^.info=_line) and not (mo1^.ylien=NIL) and not (mo1^.xlien^.ylien=NIL)
             then with volume[id_cone] do
               begin
                    sommets[1].xe:=_h*(mo1^.xreal+_xo)+(1-_h)*_xs;
                    sommets[1].ye:=_h*(mo1^.yreal+_yo)+(1-_h)*_ys;
                    sommets[1].ze:=_h*(mo1^.zreal+_zo)+(1-_h)*_zs;
                    sommets[2].xe:=_h*(mo1^.ylien^.xreal+_xo)+(1-_h)*_xs;
                    sommets[2].ye:=_h*(mo1^.ylien^.yreal+_yo)+(1-_h)*_ys;
                    sommets[2].ze:=_h*(mo1^.ylien^.zreal+_zo)+(1-_h)*_zs;
                    sommets[3].xe:=_h*(mo1^.xlien^.ylien^.xreal+_xo)+(1-_h)*_xs;
                    sommets[3].ye:=_h*(mo1^.xlien^.ylien^.yreal+_yo)+(1-_h)*_ys;
                    sommets[3].ze:=_h*(mo1^.xlien^.ylien^.zreal+_zo)+(1-_h)*_zs;
                    sommets[4].xe:=_h*(mo1^.xlien^.xreal+_xo)+(1-_h)*_xs;
                    sommets[4].ye:=_h*(mo1^.xlien^.yreal+_yo)+(1-_h)*_ys;
                    sommets[4].ze:=_h*(mo1^.xlien^.zreal+_zo)+(1-_h)*_zs;
                    ZFillPoly(sommets);
                    sommets[1].ze:=_h*(-mo1^.zreal+_zo)+(1-_h)*_zs;
                    sommets[2].ze:=_h*(-mo1^.ylien^.zreal+_zo)+(1-_h)*_zs;
                    sommets[3].ze:=_h*(-mo1^.xlien^.ylien^.zreal+_zo)+(1-_h)*_zs;
                    sommets[4].ze:=_h*(-mo1^.xlien^.zreal+_zo)+(1-_h)*_zs;
                    ZFillPoly(sommets);
               end;                                  { le polygone � 4 c�t�s est constitu�    }
            mo1:=mo1^.xlien;                         { du point, de son successeur en Y,      }
       end;                                          { du successeur en Y du successeur en X, }
                                                     { et du successeur en X.                 }
     Omega(id_cone);
     {Horloge(74,31*fontsize,black,black); }
     canvas.Pen.Mode := pmXor;
     Axes;
end;

procedure Generation(id_cone: byte);                            { m�morisation de la conique. }
var       x,y: integer; mo1,mo2: point; overflow: boolean;
          ho : array[1..1+_borne*2] of point; hj: byte;

          function Equation: real;
          begin
               with volume[id_cone] do
                    Equation:=(_k-_a*sqr(x)-_b*sqr(y))/_c;
          end;

          procedure Memorisation(nature: byte);
          begin
               if MemAvail>=SizeOf(chaine) then
                 with volume[id_cone] do
                   begin
                       inc(segments[id_cone]);
                       new(mo2);                      { pour un cha�nage en X. }
                       mo1^.info:=nature;
                       mo1^.xreal:=x;
                       mo1^.yreal:=y;
                       mo1^.zreal:=sqrt(Equation);
                       mo1^.xlien:=mo2;               { cha�nage en X. }
                       mo1^.ylien:=ho[hj];            { cha�nage en Y. }
                       ho[hj]:=mo1;
                       mo1:=mo2;
                   end
               else
                   begin
                        {OutOfMemory;}
                        overflow:=true;
                   end;
          end;

begin
     { Horloge(74,31*fontsize,yellow,lightcyan);
     SetWriteMode(CopyPut); }
     overflow:= false;
     for hj:=1 to 1+_borne*2 do                  { dans le tableau (ho) sont m�moris�s les    }
         ho[hj]:=NIL;                            { pointeurs sur les points qui appartiennent }
     segments[id_cone]:=1;                       { � la ligne pr�c�dente. }
     new(mo1);
     mo[id_cone]:=mo1;
     y:=-_borne;
     while (y<=_borne) and not overflow do
       begin
            x:=-_borne;
            while (Equation <0) and (x<=_borne) do
                  inc(x,volume[id_cone]._pas);
            if x<=_borne then
               begin
                    hj:= 1;
                    Memorisation(_move);
                    inc(x,volume[id_cone]._pas);
               end;
            while (x<=_borne) and not overflow do
              begin
                   inc(hj);
                   if not (Equation<0) then         { on �limine les �(x<0). }
                      Memorisation(_line)
                   else
                       ho[hj]:=NIL;
                   inc(x,volume[id_cone]._pas);
              end;
            inc(y,volume[id_cone]._pas);
       end;
     mo1^.info:=_stop;                              { marque la fin de la cha�ne. }
     mo1^.xreal:=0;
     mo1^.yreal:=0;
     mo1^.zreal:=0;
     mo1^.xlien:=NIL;
     mo1^.ylien:=NIL;
     {Horloge(74,31*fontsize,black,black);
     SetWriteMode(XorPut);}
end;

procedure Desallocation(id_cone: byte);             { supprime l'objet conique de la m�moire. }
var       mo1: point; desalloue: boolean;
begin
     desalloue:=false;
     mo1:=mo[id_cone];
     repeat
           if mo1^.info=_stop then desalloue:=true;
           dispose(mo1);
           if not desalloue then mo1:=mo1^.xlien;
     until desalloue;
end;

function Validation(var c1,c2,c3: real): byte;        { [in]valide le param�trage en fonction }
const    Tconic: array[-1..+1,-1..+1,-1..+1] of byte= { des signes des coefficients a,b et c. }
               ( ((0,0,3),(0,0,5),(3,5,2)),
                 ((0,0,5),(0,0,6),(5,6,4)),
                 ((3,5,2),(5,6,4),(2,4,1)) );

         function Signe(var reel: real): shortint;
         begin
              if reel > 0 then
                 Signe:= +1
         else
              if reel < 0 then
                 Signe:= -1
         else
                 Signe:= 0;
         end;
begin
     Validation:= Tconic[Signe(c1),Signe(c2),Signe(c3)];
end;

procedure Attributs;                            { affiche les param�tres de la conique n� id. }
const     _step: array[1..2] of string[6]=(' pas =',' step=');
begin
    with volume[id] do
       begin
       {
           GotoXY(1, 2); TextColor(lightred); Write('a'); TextColor(white); Write('=',_a:9:2);
           GotoXY(1, 3); TextColor(lightred); Write('b'); TextColor(white); Write('=',_b:9:2);
           GotoXY(1, 4); TextColor(lightred); Write('c'); TextColor(white); Write('=',_c:9:2);
           GotoXY(1, 5); TextColor(lightred); Write('k'); TextColor(white); Write('=',_k:9:2);
           GotoXY(1, 7); TextColor(lightred); Write('X'); TextColor(white); Write('=',_xo:9:2);
           GotoXY(1, 8); TextColor(lightred); Write('Y'); TextColor(white); Write('=',_yo:9:2);
           GotoXY(1, 9); TextColor(lightred); Write('Z'); TextColor(white); Write('=',_zo:9:2);
           GotoXY(1,11); TextColor(lightred); Write('h'); TextColor(white); Write('=',_h:9:2);
           GotoXY(1,12); TextColor(lightred); Write('u'); TextColor(white); Write('=',_xs:9:2);
           GotoXY(1,13); TextColor(lightred); Write('v'); TextColor(white); Write('=',_ys:9:2);
           GotoXY(1,14); TextColor(lightred); Write('w'); TextColor(white); Write('=',_zs:9:2);
           GotoXY(1,26); TextColor(lightred); Write('F0');
                         TextColor(white);    Write(_step[langue],volume[id]._pas:3);
       }
       end;
end;


procedure StartUp;
begin
     {orgmode:= lastmode;
     Pays;
     ModeGraphique;
     SetTextStyle(DefaultFont, HorizDir,2);}
     {DirectVideo:= false;                   { astuce pour un affichage du texte via le BIOS. }
     langue := 1;
     memtotale:=MemAvail;
     depth:=$200;                           { la profondeur. }
     xshift:=0;                             { variables de translation. }
     yshift:=0;
     zshift:=0;
     for id:=1 to _volumes do
         begin
              volume[id]:= _volume[id];
              Generation(id);
              nom[id]:= Validation(_volume[id]._a,_volume[id]._b,_volume[id]._c);
         end;
     id:=1;
     xangle:=236; yangle:=220; zangle:=204;
     Trigo(xangle,yangle,zangle);


     {Auteur;
     Information;

     repeat                                                    { boucle principale de saisie.
           choix:= UpCase(readkey);
           case choix of
           'A': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,2); Write('         ');
                     GotoXY(3,2); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        begin
                             xnom:=Validation(chif6,volume[id]._b,volume[id]._c);
                             if not (xnom=0) then
                                begin
                                     nom[id]:=xnom;
                                     volume[id]._a:= chif6;
                                     Desallocation(id);
                                     Generation(id);
                                end
                             else Insoluble;
                        end
                     else Invalide;
                     GotoXY(3,2); Write(volume[id]._a:9:2);
                end;
           'B': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,3); Write('         ');
                     GotoXY(3,3); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        begin
                             xnom:=Validation(volume[id]._a,chif6,volume[id]._c);
                             if not (xnom=0) then
                                begin
                                     nom[id]:=xnom;
                                     volume[id]._b:=chif6;
                                     Desallocation(id);
                                     Generation(id);
                                end
                             else Insoluble;
                        end
                     else Invalide;
                     GotoXY(3,3); Write(volume[id]._b:9:2);
                end;
           'C': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,4); Write('         ');
                     GotoXY(3,4); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if (testnum= 0) and not (chif6=0) then   { division par z�ro impossible.
                        begin
                             xnom:=Validation(volume[id]._a,volume[id]._b,chif6);
                             if not (xnom=0) then
                                begin
                                     nom[id]:=xnom;
                                     volume[id]._c:=chif6;
                                     Desallocation(id);
                                     Generation(id);
                                end
                             else Insoluble;
                        end
                     else Invalide;
                     GotoXY(3,4); Write(volume[id]._c:9:2);
                end;
           'E': if langue= _francais then
                   begin                                 { efface la zone de trac� graphique.
                        ClearViewPort;
                        Axes;
                   end;
           'H': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,11); Write('         ');
                     GotoXY(3,11); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._h:=chif6
                     else Invalide;
                     GotoXY(3,11); Write(volume[id]._h:9:2);
                end;
           'I': Information;
           'K': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,5); Write('         ');
                     GotoXY(3,5); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        begin
                             volume[id]._k:=chif6;
                             Desallocation(id);
                             Generation(id);
                        end
                     else Invalide;
                     GotoXY(3,5); Write(volume[id]._k:9:2);
                end;
           'L': if langue= _anglais then
                   begin                                         { clear drawing view screen.
                        ClearViewPort;
                        Axes;
                   end;
           'U': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,12); Write('         ');
                     GotoXY(3,12); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._xs:=chif6
                     else Invalide;
                     GotoXY(3,12); Write(volume[id]._xs:9:2);
                end;
           'V': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,13); Write('         ');
                     GotoXY(3,13); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._ys:=chif6
                     else Invalide;
                     GotoXY(3,13); Write(volume[id]._ys:9:2);
                end;
           'W': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,14); Write('         ');
                     GotoXY(3,14); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._zs:=chif6
                     else Invalide;
                     GotoXY(3,14); Write(volume[id]._zs:9:2);
                end;
           'X': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,7); Write('         ');
                     GotoXY(3,7); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._xo:=chif6
                     else Invalide;
                     GotoXY(3,7); Write(volume[id]._xo:9:2);
                end;
           'Y': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,8); Write('         ');
                     GotoXY(3,8); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._yo:=chif6
                     else Invalide;
                     GotoXY(3,8); Write(volume[id]._yo:9:2);
                end;
           'Z': begin
                     SetTextBuf(InPut,buffer2);
                     TextColor(white);
                     GotoXY(3,9); Write('         ');
                     GotoXY(3,9); Readln(buffer);
                     Val(buffer,chif6,testnum);
                     if testnum= 0 then
                        volume[id]._zo:=chif6
                     else Invalide;
                     GotoXY(3,9); Write(volume[id]._zo:9:2);
                end;
           '+': if zshift<300 then
                   begin
                        Axes;
                        inc(zshift,2);
                        TextColor(white);
                        GotoXY(9,25); Write(zshift:3);
                        Axes;
                   end;
           '-': if zshift>0 then
                   begin
                        Axes;
                        dec(zshift,2);
                        TextColor(white);
                        GotoXY(9,25); Write(zshift:3);
                        Axes;
                   end;
           #00: begin                             { pour saisir au clavier les codes �tendus.
                     choix:= ReadKey;
                     case choix of
                     _left : begin
                                  Axes;
                                  dec(xshift,_kshift);
                                  TextColor(white);
                                  GotoXY(6,16); Write(xshift:6);
                                  Axes;
                             end;
                     _right: begin
                                  Axes;
                                  inc(xshift,_kshift);
                                  TextColor(white);
                                  GotoXY(6,16); Write(xshift:6);
                                  Axes;
                             end;
                     _up   : begin
                                  Axes;
                                  dec(yshift,_kshift);
                                  TextColor(white);
                                  GotoXY(6,17); Write(yshift:6);
                                  Axes;
                             end;
                     _down : begin
                                  Axes;
                                  inc(yshift,_kshift);
                                  TextColor(white);
                                  GotoXY(6,17); Write(yshift:6);
                                  Axes;
                             end;
                     _f1   : begin
                                  Axes;
                                  dec(xangle,4); if xangle<0 then xangle:=356;
                                  Trigo(xangle,yangle,zangle);
                                  TextColor(white);
                                  GotoXY(9,20); Write(xangle:3);
                                  Axes;
                             end;
                     _f2   : begin
                                  Axes;
                                  inc(xangle,4); if xangle>359 then xangle:=0;
                                  Trigo(xangle,yangle,zangle);
                                  TextColor(white);
                                  GotoXY(9,20); Write(xangle:3);
                                  Axes;
                             end;
                     _f3   : begin
                                  Axes;
                                  dec(yangle,4); if yangle<0 then yangle:=356;
                                  Trigo(xangle,yangle,zangle);
                                  TextColor(white);
                                  GotoXY(9,21); Write(yangle:3);
                                  Axes;
                             end;
                     _f4   : begin
                                  Axes;
                                  inc(yangle,4); if yangle>359 then yangle:=0;
                                  Trigo(xangle,yangle,zangle);
                                  TextColor(white);
                                  GotoXY(9,21); Write(yangle:3);
                                  Axes;
                             end;
                     _f5   : begin
                                  Axes;
                                  dec(zangle,4); if zangle<0 then zangle:=356;
                                  Trigo(xangle,yangle,zangle);
                                  TextColor(white);
                                  GotoXY(9,22); Write(zangle:3);
                                  Axes;
                             end;
                     _f6   : begin
                                  Axes;
                                  inc(zangle,4); if zangle>359 then zangle:=0;
                                  Trigo(xangle,yangle,zangle);
                                  TextColor(white);
                                  GotoXY(9,22); Write(zangle:3);
                                  Axes;
                             end;
                     _f0   : begin
                                  SetTextBuf(InPut,buffer3);
                                  TextColor(white);
                                  GotoXY(9,26); Write('   ');
                                  GotoXY(9,26); Readln(buffer);
                                  Val(buffer,chif3,testnum);
                                  if (testnum= 0) and (chif3>=_pas_min) and (chif3<=_borne*2)
                                   then
                                     begin
                                          volume[id]._pas:=chif3;
                                          Desallocation(id);
                                          Generation(id);
                                     end
                                  else Invalide;
                                  GotoXY(9,26); Write(volume[id]._pas:3);
                             end;
                     end;
                end;
           end;
     until choix= _esc;}
end;

procedure ShutDown;
begin
     for id:=1 to _volumes do               { on supprime toutes les coniques en m�moire. }
         Desallocation(id);
     {CloseGraph;                            { restaure le mode video initial.
     TextMode(orgmode);}
end;

