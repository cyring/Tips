unit Winconic;

interface

uses
  SysUtils, WinTypes, WinProcs, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, StdCtrls;

type
  TFormConic = class(TForm)
    ButtonTracer: TButton;
    ComboConics: TComboBox;
    ButtonEffacer: TButton;
    ButtonPlein: TButton;
    ButtonInfo: TButton;
    ListBoxInfo: TListBox;
    procedure FormOnShow(Sender: TObject);
    procedure FormOnResize(Sender: TObject);
    procedure FormOnCreate(Sender: TObject);
    procedure ButtonTracerClick(Sender: TObject);
    procedure ComboConicsClick(Sender: TObject);
    procedure ButtonEffacerClick(Sender: TObject);
    procedure FormActivate(Sender: TObject);
    procedure ButtonPleinClick(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure ButtonInfoClick(Sender: TObject);
         private
         { Private-déclarations }
         public
         { Public-déclarations }
  end;

var
  FormConic: TFormConic;
  canvas: TCanvas;

implementation

{$R *.DFM}

{$I CONIQUES.PAS}

procedure TFormConic.FormOnShow(Sender: TObject);
var       S : string[16];
begin
     canvas.Font.Name := 'Terminal';
     canvas.Font.Size := 9;
     canvas.Pen.Color := clBlack;
     canvas.MoveTo(92,0);
     canvas.LineTo(92,GetMaxY);
{
     canvas.Font.Color := clBlue;
     canvas.TextOut(1, 1, _conique[langue]);
     canvas.Font.Color := clRed;
     canvas.TextOut(canvas.PenPos.X, 1, 'n');
     canvas.Font.Color := clBlue;
     canvas.TextOut(canvas.PenPos.X, 1, 'ø');
     canvas.Font.Color := clWhite;
     str(id:1, S);
     canvas.TextOut(canvas.PenPos.X, 1, S);
}
     Axes;
end;

procedure TFormConic.FormOnResize(Sender: TObject);
begin
     FormConic.refresh;
end;

procedure TFormConic.FormOnCreate(Sender: TObject);
begin
  Startup;
end;

procedure TFormConic.ButtonTracerClick(Sender: TObject);
begin
     Trace1(id);
end;



procedure TFormConic.ComboConicsClick(Sender: TObject);
begin
     if ComboConics.ItemIndex <> -1 then
        begin
             id := ComboConics.ItemIndex + 1;
             Attributs;
        end
end;

procedure TFormConic.ButtonEffacerClick(Sender: TObject);
begin
     FormConic.refresh;
end;

procedure TFormConic.FormActivate(Sender: TObject);
var       i : integer;
begin
     {
     GotoXY(1, 6); TextColor(lightblue); Write(_origine[langue]);
     GotoXY(1,10); TextColor(lightblue); Write(_homo[langue]);
     Attributs;
     GotoXY(1,15); TextColor(lightblue); Write('translation');
     GotoXY(1,16); TextColor(lightred);  Write(#27#26); TextColor(white); Write(' x=',xshift:6);
     GotoXY(1,17); TextColor(lightred);  Write(#24#25); TextColor(white); Write(' y=',yshift:6);
     GotoXY(1,18); TextColor(lightblue);  Write('  Rotation');
     GotoXY(1,19);                        Write('-1 +1 angle');
     GotoXY(1,20); TextColor(lightred);   Write('F1 F2');
                   TextColor(lightcyan);  Write(' x');
                   TextColor(white);      Write('=',xangle:3);
     GotoXY(1,21); TextColor(lightred);   Write('F3 F4');
                   TextColor(yellow);     Write(' y');
                   TextColor(white);      Write('=',yangle:3);
     GotoXY(1,22); TextColor(lightred);   Write('F5 F6');
                   TextColor(lightgreen); Write(' z');
                   TextColor(white);      Write('=',zangle:3);
     GotoXY(1,24); TextColor(lightblue); Write(_special[langue]);
     GotoXY(1,25); TextColor(lightred);  Write('-+');
                   TextColor(white);     Write(' zoom=',zshift:3);
     GotoXY(1,28); TextColor(lightblue); Write(_graph[langue]);
     GotoXY(1,34); TextColor(lightred);  Write('ESC'); TextColor(lightblue); Write(_fin[langue]);}
     case langue of
      _francais:
       begin
            ButtonEffacer.Caption := 'Effacer';
            ButtonTracer.Caption := 'Tracer';
            ButtonPlein.Caption := 'Plein';
       end;
      _anglais:
       begin
            ButtonEffacer.Caption := 'Clear';
            ButtonTracer.Caption := 'Outlined';
            ButtonPlein.Caption := 'Solid';
       end;
     end;
     ComboConics.Text := _conic[1,langue];
     for i := 1 to 6 do
         ComboConics.Items.Add(_conic[i,langue]);
end;

procedure TFormConic.ButtonPleinClick(Sender: TObject);
begin
          Trace2(id);
end;

procedure TFormConic.FormDestroy(Sender: TObject);
begin
          ShutDown;
end;

procedure TFormConic.ButtonInfoClick(Sender: TObject);
begin
          if ListBoxInfo.Visible = True then
             ListBoxInfo.Visible := False
          else
              begin
                   ListBoxInfo.Visible := True;
                   ListBoxInfo.Clear;
                   Information;
              end;
end;

end.
