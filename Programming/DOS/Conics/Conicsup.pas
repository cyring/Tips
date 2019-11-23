{                             ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                              ³ Auteur : COURTIAT Cyril  CSII-1A ³
                              ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}

procedure InitPolice;                              { initialise la police de caractŠres 8x14. }
var       register: registers;
begin
     with register do
       begin
            ah:=17;
            al:=48;
            if GraphPilote in [MCGA,VGA,IBM8514,SVGA] then
               begin
                    bh:=02;
                    fontsize:=14;
               end
            else
                begin
                     bh:=03;
                     fontsize:=08;
                end;
            Intr($10,register);
            ah:=17;
            al:=33;
            bl:=00;
            cx:=fontsize;
            Intr($10,register);
       end;
end;

procedure Horloge(xh,yh: integer; cadran,aiguille: word);     { t‚moigne l'ex‚cution d'un job }
begin                                                         { ‚ventuellement long. }
     SetViewPort(0,0,GetMaxX,GetMaxY,ClipOff);
     SetColor(cadran);
     Circle(xh,yh,12);
     SetColor(aiguille);
     Line(xh,yh,xh-7,yh-7);
     Line(xh,yh,xh+8,yh);
     SetViewPort(93,0,GetMaxX,GetMaxY,ClipOn);
end;

procedure Interdit(xh,yh: integer; fond, symb: word);         
begin
     SetViewPort(0,0,GetMaxX,GetMaxY,ClipOff);
     SetColor(fond);
     SetFillStyle(SolidFill,fond);
     PieSlice(xh,yh,0,360,12);
     SetColor(symb);
     SetFillStyle(SolidFill,symb);
     Bar(xh-8,yh-3,xh+8,yh+3);
     SetViewPort(93,0,GetMaxX,GetMaxY,ClipOn);
end;

procedure Interrogation(xh,yh: integer; fond, symb: word);
begin
     SetViewPort(0,0,GetMaxX,GetMaxY,ClipOff);
     SetColor(fond);
     Circle(xh,yh,12);
     SetColor(symb);
     MoveTo(xh-8,yh-6); OutText('?');
     SetViewPort(93,0,GetMaxX,GetMaxY,ClipOn);
end;

procedure Memoire(xh,yh: integer; fond, symb: word);
begin
     SetViewPort(0,0,GetMaxX,GetMaxY,ClipOff);
     SetColor(fond);
     SetFillStyle(SolidFill,fond);
     PieSlice(xh,yh,0,360,12);
     SetColor(symb);
     MoveTo(xh-7,yh-7); OutText('M');
     SetViewPort(93,0,GetMaxX,GetMaxY,ClipOn);
end;

procedure HipHop;                                          { signale sonore. }
begin
     Sound(50);
     Delay(120);
     NoSound;
     Delay(120);
     Sound(50);
     Delay(120);
     NoSound;
end;

procedure Invalide;         { t‚moigne une saisie invalide (num‚ricit‚ ou division par z‚ro). }
begin
     Interdit(74,31*fontsize,lightred,white);
     HipHop;
     Interdit(74,31*fontsize,black,black);
end;

procedure Insoluble;          { t‚moigne la saisie de paramŠtres d'une conique sans solution. }
begin
     Interrogation(74,31*fontsize,lightred,white);
     HipHop;
     Interrogation(74,31*fontsize,black,black);
end;

procedure OutOfMemory;             { t‚moigne un d‚passement de capacit‚ m‚moire dans le tas. }
begin
     Memoire(74,31*fontsize,lightblue,white);
     HipHop;
     Memoire(74,31*fontsize,black,black);
end;

procedure Auteur;
const     _titre : array[1..2] of string[39]=('³           C O N I Q U E S           ³',
                                              '³             C O N I C S             ³');
          _auteur: array[1..2] of string[6]=('Auteur','Author');
begin
     TextColor(lightblue);
     GotoXY(14,2); Write('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
     GotoXY(14,3); Write(_titre[langue]);
     GotoXY(14,4); Write('ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´');
     GotoXY(14,5); Write('³  ',_auteur[langue],'                     FRANCE  ³');
     GotoXY(14,6); Write('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
     TextColor(lightred);
     GotoXY(26,5); Write('COURTIAT Cyril');
end;

Procedure Pays;
const     tong: array[1..2] of string[36]=('         Je parle fran‡ais          ',
                                           '         I speak english            ');
          util: array[1..9,1.._langues] of string[74]=(
                ('Les touches … actionner ont une couleur rouge, les titres de section sont ',
                 'The action keys are displayed in red, the section titles in blue and the  '),
                ('en bleu et les informations variables sont en blanc suivant le modŠle     ',
                 'variables in white as a function of the mathematic model a.Xý+b.Yý+c.Zý=k '),
                ('math‚matique a.Xý + b.Yý + c.Zý = k. Le trac‚ de la conique a deux repr‚- ',
                 'The conic is represented in two ways: the skeleton (outline) or the filled'),
                ('sentations : le squelette (trac‚ vide) ou la forme pleine (trac‚ plein).  ',
                 'form (solid). The three dimensional designs are oriented by the axes x,y,z'),
                ('La zone de trac‚ comprend les axes de direction x,y,z. L''homot‚thie a un  ',
                 'The homothetia is defined by the factor h and its center (u,v,w), repre-  '),
                ('facteur h et un centre (u,v,w) symbolis‚ par un carr‚ blanc.En bas du menu',
                 'sented by a white square. Four information icons appear at the bottom of  '),
                ('apparaissent quatre icones d''information:le sens interdit (mauvaise saisie',
                 'the menu: the no thoroughfare (bad input or coefficent c=0), the question-'),
                ('ou coefficient c=0), le point d''interrogation (param‚tres sans solution), ',
                 'mark (no solution with input parameters), the capital m (out of memory),  '),
                ('le m majuscule (plus assez de m‚moire) et l''horloge (traitement en cours). ',
                 'and the clock (computing or drawing).                                     '));

var       regs: registers;
          info: array[0..34] of char;
          choix: char;

          procedure InActif;
          begin
               TextAttr:=lightgray Shl 4 + blue;;
               GotoXY(22,07+langue); Write(tong[langue]);
          end;

          procedure Actif;
          var       ligne: byte;
          begin
               TextAttr:=blue Shl 4 + white;
               GotoXY(22,07+langue); Write(tong[langue]);
               TextAttr:=blue Shl 4 + blue;
               GotoXY(22,07+langue); Write(#32+#08);
               TextAttr:=lightgray Shl 4 + blue;
               for ligne:=1 to 9 do
               begin
                    GotoXY(02,ligne+13);
                    Write('³',util[ligne,langue],'³');
               end;
          end;

begin
     TextMode(CO80);
     TextAttr:=red Shl 4 + red;
     ClrScr;
     TextAttr:=black;
     Window(23,03,60,11);
     ClrScr;
     Window(04,14,79,24);
     ClrScr;
     TextAttr:=lightgray Shl 4 + blue;
     Window(01,01,80,25);
     GotoXY(21,02); Write('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
     GotoXY(21,03); Write('³ ÖÄÄ ÖÄÄ¿ Ö¿ ¿ Ò ÖÄÄ¿ Ö  ¿ ÖÄÄ ÖÄÄ  ³');
     GotoXY(21,04); Write('³ º   º  ³ ºÀ¿³ º ÓÄÄ´ º  ³ ÇÄ  ÓÄÄ¿ ³');
     GotoXY(21,05); Write('³ ÓÄÄ ÓÄÄÙ Ó ÀÙ Ð    Ù ÓÄÄÙ ÓÄÄ ÓÄÄÙ ³');
     GotoXY(21,06); Write('³  (C) 1992           COURTIAT Cyril ³');
     GotoXY(21,07); Write('ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´');
     GotoXY(21,08); Write('³',tong[_francais],'³');
     GotoXY(21,09); Write('³',tong[_anglais],'³');
     GotoXY(21,10); Write('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
     GotoXY(02,13);
     Write('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
     GotoXY(02,23);
     Write('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
     with regs do
     begin
          ax:=$3800;
          ds:=Seg(info);
          dx:=Ofs(info);
          MsDos(regs);
          if bx in [002,032,033,041] then
               langue:= _francais
          else langue:= _anglais;
     end;
     Actif;
     repeat
           choix:= readkey;
           case choix of
           _esc : begin
                       TextAttr:= white;
                       TextMode(orgmode);
                       halt(3);
                  end;
            #00 : begin
                       choix:= readkey;
                       case choix of
                       _up   : if langue > _francais then
                                  begin
                                       InActif;
                                       Dec(langue);
                                       Actif;
                                  end;
                       _down : if langue < _langues  then
                                  begin
                                       InActif;
                                       Inc(langue);
                                       Actif;
                                  end;
                       end;
                  end;
           end;
     until choix= _enter;
end;

procedure ModeGraphique;
type      _grafrec= record
                    name: string[39];
                    driv,
                    mode: integer;
          end;
const     _pilote : array[1.._langues] of string[27]
                                     =('Graphique CGA non support‚.',
                                       'CGA graphic is not allowed.');
          _gflist : array[1..14] of _grafrec= (
           (name:'    MCGA           640 x 480        '; driv:MCGA;     mode:MCGAhi),
           (name:'    EGA            640 x 350        '; driv:EGA;      mode:EGAHi ),
           (name:'    EGA64          640 x 350        '; driv:EGA64;    mode:EGA64Hi),
           (name:'    EGAMono        640 x 350        '; driv:EGAMono;  mode:EGAMonoHi),
           (name:'    HercMono       720 x 348        '; driv:HercMono; mode:HercMonoHi),
           (name:'    VGA            640 x 350        '; driv:VGA;      mode:VGAMed),
           (name:'    VGA            640 x 480        '; driv:VGA;      mode:VGAHi),
           (name:'    ATT400         640 x 400        '; driv:ATT400;   mode:ATT400Hi),
           (name:'    PC3270         720 x 350        '; driv:PC3270;   mode:PC3270Hi),
           (name:'    IBM8514        640 x 480        '; driv:IBM8514;  mode:IBM8514Lo),
           (name:'    IBM8514       1024 x 768        '; driv:IBM8514;  mode:IBM8514Hi),
           (name:'    SuperVGA       640 x 480        '; driv:SVGA;     mode:SVGALo),
           (name:'    SuperVGA       800 x 600        '; driv:SVGA;     mode:SVGAMed),
           (name:'    SuperVGA      1024 x 768        '; driv:SVGA;     mode:SVGAHi) );
          _BGItable : array[2..10] of byte= (1,2,3,4,11,5,8,7,9);
          _graftitle: array[1.._langues] of string[39]
                                         =('   S‚lectionner un mode graphique   ',
                                           '     Select a graphic resolution    ');

var       ligne: byte;
          choix: char;
          TestPilote: integer;

          { Fonction d'autod‚tection.
            Suppose que le mat‚riel est toujours pr‚sent.
            Valeur renvoy‚e = mode par d‚faut recommand‚. }

          {$F+}
          function TestDetect: integer;
          begin
               TestDetect := SvgaMode;
          end;
          {$F-}

begin
     DetectGraph(GraphPilote,GraphMode);
     if GraphPilote in [CGA] then
        begin
             TextMode(orgmode);
             writeln(_pilote[langue]);
             Halt(2);
        end;
     if ParamCount > 0 then
         BGIpath:= ParamStr(1)
     else
         BGIpath:='';
     TextAttr:=red Shl 4 + red;
     ClrScr;
     TextAttr:=black;
     Window(23,03,60,05);
     ClrScr;
     Window(23,09,60,24);
     ClrScr;
     TextAttr:=lightgray Shl 4 + blue;
     Window(01,01,80,25);
     GotoXY(21,02); Write('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
     GotoXY(21,03); Write('³',_graftitle[langue],'³');
     GotoXY(21,04); Write('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
     GotoXY(21,08); Write('ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿');
     GotoXY(21,23); Write('ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');
     for ligne:=1 to 14 do
     begin
          GotoXY(21,ligne+8); Write('³',_gflist[ligne].name,'³');
     end;
     ligne:= _BGItable[GraphPilote];
     TextAttr:=blue Shl 4 + white;
     GotoXY(22,ligne+8); Write(_gflist[ligne].name,#8);
     TextAttr:=blue Shl 4 + blue;
     GotoXY(22,ligne+8); Write(#32,#8);
     repeat
           choix:= readkey;
           case choix of
           _esc : begin
                       TextAttr:= white;
                       TextMode(orgmode);
                       halt(3);
                  end;
            #00 : begin
                       choix:= readkey;
                       case choix of
                       _up   : if ligne > 1 then
                                  begin
                                       TextAttr:=lightgray Shl 4 + blue;
                                       GotoXY(22,ligne+8); Write(_gflist[ligne].name);
                                       Dec(ligne);
                                       TextAttr:=blue Shl 4 + white;
                                       GotoXY(22,ligne+8); Write(_gflist[ligne].name);
                                       TextAttr:=blue Shl 4 + blue;
                                       GotoXY(22,ligne+8); Write(#32,#8);
                                  end;
                       _down : if ligne < 14  then
                                  begin
                                       TextAttr:=lightgray Shl 4 + blue;
                                       GotoXY(22,ligne+8); Write(_gflist[ligne].name);
                                       Inc(ligne);
                                       TextAttr:=blue Shl 4 + white;
                                       GotoXY(22,ligne+8); Write(_gflist[ligne].name,#8);
                                       TextAttr:=blue Shl 4 + blue;
                                       GotoXY(22,ligne+8); Write(#32,#8);
                                  end;
                       end;
                  end;
           end;
     until choix= _enter;
     TextMode(lastmode+font8x8);
     if _gflist[ligne].driv = 255 then
        begin
             TestPilote := InstallUserDriver('SVGA256', @TestDetect);
             if GraphResult <> grOk then
                begin
                     NormVideo;
                     TextMode(orgmode);
                     Writeln(GraphErrorMsg(GraphErreur));
                     Halt(1);
                end
             else
                 GraphPilote := Detect;
             SvgaMode:= _gflist[ligne].mode;
             InitGraph(GraphPilote, GraphMode,BGIpath);
        end
     else
         InitGraph(_gflist[ligne].driv, _gflist[ligne].mode,BGIpath);
     GraphErreur:= GraphResult;
     if GraphErreur <> grOK then
     begin
          TextMode(orgmode);
          WriteLn(GraphErrorMsg(GraphErreur));
          Halt(1);
     end;
     InitPolice;
end;
