{                     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                      ³ Auteur : COURTIAT Cyril  CSII-1A ³
                      ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}

unit ZGraph;

interface

uses      WinTypes, Graphics;

const     GetMaxX: integer = 800;
          GetMaxY: integer = 600;
type
          ZPolyPoints= array[1..4] of record
                                       xe,
                                       ye,
                                       ze: real;
                                      end;

var       depth,xshift,yshift,zshift: integer;
          a,b,c,d,e,f,g,h,i: real;

procedure Trigo(xa,ya,za: integer);
procedure Zmove(xe,ye,ze: real);
procedure Zplot(xe,ye,ze: real; pixel: TColor);
procedure Zline(xe1,ye1,ze1,xe2,ye2,ze2: real; pixel: TColor);
procedure ZlineTo(xe,ye,ze: real; pixel: TColor);
procedure Zbox(xe1,ye1,ze1,xe2,ye2,ze2: real; pixel: TColor);
procedure ZFillPoly(sommets: ZPolyPoints);

implementation

var       a,b,c,d,e,f,g,h,i: real;


procedure Trigo;                  { calcule des paramŠtres de rotation. }

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

procedure Zmove;                  { positionne le curseur graphique en 3D. }
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

procedure Zplot;                  { allume un point de couleur pixel. }
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

procedure Zline;                  { trace une ligne de couleur pixel. }
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
     canvas.Brush.Color := pixel;
     canvas.MoveTo(trunc(xproj1),trunc(yproj1));
     canvas.LineTo(trunc(xproj2),trunc(yproj2));
end;

procedure ZlineTo;                { trace une ligne de couleur pixel. }
var       q,xproj,yproj,xrot,yrot,zrot: real;
begin
     xrot:= a*xe+d*ye+g*ze+xshift;
     yrot:= b*xe+e*ye+h*ze+yshift;
     zrot:= c*xe+f*ye+i*ze+zshift;
     q:= 1-(zrot/depth);                         { cf Zmove. }
     xproj:= xrot/q + GetMaxX / 2;
     yproj:= yrot/q + GetMaxY / 2;
     canvas.Brush.Color := pixel;
     canvas.LineTo(trunc(xproj),trunc(yproj)); { relativement au curseur graphique. }
end;

procedure Zbox;                   { trace en 3D une boite de couleur pixel. }
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

procedure ZFillPoly;              { trace un polygone … quatre c“t‚s. }
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
     canvas.Polyline(proj);
end;


begin
end.
