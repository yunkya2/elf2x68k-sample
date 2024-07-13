   10 /* Mexican hat
   20 screen 0,0,1,1:cls:contrast(15)
   30 int i,x,y,sx,sy,zz
   40 float dr,r,z
   50 dim d(160)
   60 for i=0 to 160:d(i)=100:next
   70 dr=pi()/180
   80 for y=-180 to 180
   90   for x=-180 to 180
  100     r=dr*sqr(x*x+y*y)
  110     z=100*cos(r)-30*cos(3*r)
  120     sx=int(80+x/3-y/6)
  130     sy=int(40-y/6-z/4)
  140     if (sx>=0) and (sx<160) then {
  150       if d(sx) > sy then {
  160         zz=int((z+100)*0.035#)+1
  170         pset(sx*256/160,sy*256/100,zz*2+1)
  180         d(sx)=sy
  190       }
  200     }
  210   x=x+3
  220   next
  230 y=y+5
  240 next
  250 print "Hit any key.";inkey$
  260 width 96
