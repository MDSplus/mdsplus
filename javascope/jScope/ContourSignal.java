package jScope;

import java.awt.geom.Point2D;
import java.io.IOException;
import java.util.Vector;

public class ContourSignal
{
  static final int CASE_A = 0;
  static final int CASE_B = 1;
  static final int CASE_C = 2;
  static final int CASE_D = 3;

  private static int xNear_A[] =
      {
      0, 1, 1, 0};
  private static int yNear_A[] =
      {
      0, 0, 1, 1};
  private static int rPoint_A[] =
      {
      0, 1, 3};
  private static int succCase_A[] =
      {
      CASE_C, CASE_A, CASE_B};

  private static int xNear_B[] =
      {
      1, 1, 0, 0};
  private static int yNear_B[] =
      {
      0, 1, 1, 0};
  private static int rPoint_B[] =
      {
      0, 2, 2};
  private static int succCase_B[] =
      {
      CASE_A, CASE_B, CASE_D};

  private static int xNear_C[] =
      {
      0, 0, 1, 1};
  private static int yNear_C[] =
      {
      0, -1, -1, 0};
  private static int rPoint_C[] =
      {
      0, 1, 2};
  private static int succCase_C[] =
      {
      CASE_D, CASE_C, CASE_A};

  private static int xNear_D[] =
      {
      0, -1, -1, 0};
  private static int yNear_D[] =
      {
      0, 0, -1, -1};
  private static int rPoint_D[] =
      {
      1, 1, 2};
  private static int succCase_D[] =
      {
      CASE_B, CASE_D, CASE_C};

  double x[];
  float y[];
  float z[][];

  double xmin = -1, xmax = 1;
  double ymin = -1, ymax = 1;
  double zmin = -1, zmax = 1;

  boolean automaticLimits = true;

  private boolean xflag[][];
  private boolean equalZ2;

  ContourSignal(Signal s)
  {
	if(s.getType() == Signal.TYPE_2D)
	{
	    this.setMinMaxX(s.getX2Dmin(), s.getX2Dmax());
	    this.setMinMaxY(s.getY2Dmin(), s.getY2Dmax());
	    this.setMinMaxZ(s.getZ2Dmin(), s.getZ2Dmax());
            this.x = s.getX2D();
            this.y = s.getY2D();
            this.z = s.getZ2D();
	}
  }

  ContourSignal(double x[], float y[], float z[][])
  {

    if (x.length != z.length || y.length != z[0].length)
    {
      throw (new IllegalArgumentException(
          "Z colum must be equals to x element end Z row to y elements"));
    }
    this.x = x;
    this.y = y;
    this.z = z;
    computeMinMax();
  }

  private final boolean checkIntersection(double level, double z1, double z2)
  {
    boolean out;

    out = (z1 < level && level < z2) ||
                (z2 < level && level < z1) ||
                (equalZ2 = (level == z2)) ;

    return out;
  }

  public Vector<Vector<Point2D.Double>> contour(double level)
  {
    Vector<Vector<Point2D.Double>> contours = new Vector<>();
    Vector<Point2D.Double> contour = new Vector<>();

    double x1, y1, z1;
    double x2, y2, z2;
    double xc, yc, c1;


//System.out.println("Livello " + level);

    int xNear[]    = null;
    int yNear[]    = null;
    int rPoint[]   = null;
    int succCase[] = null;

    Point2D.Double firstCPoint = new Point2D.Double();
    Point2D.Double currCPoint = new Point2D.Double();

    xflag = new boolean[x.length][y.length];

    int edgeCase = CASE_A;
    int ri = 0;
    int rj = 0;

    int maxIteractions = x.length * y.length;

    for (int i = 0; i < x.length; i++)
    {
      for (int j =  0; j < y.length - 1; j++)
      {
        if (xflag[i][j])
        {
          continue;
        }

        x1 = x[i];
        y1 = y[j];
        z1 = z[i][j];

        x2 = x[i];
        y2 = y[j + 1];
        z2 = z[i][j + 1];

        //xflag[i][j] = true;
        //System.out.println("Manin Case A set "+i+" "+j);

        if ( checkIntersection(level, z1, z2) )
        {
          c1 = (level - z1) / (z2 - z1);
          xc = x1 + (x2 - x1) * c1;
          yc = y1 + (y2 - y1) * c1;
          contour.addElement( ( firstCPoint = new Point2D.Double(xc, yc) ) );
          edgeCase = CASE_A;
          ri = i;
          rj = j;
          if(equalZ2)
          {
            try
            {
              xflag[i][j-1] = true;
              xflag[i][j]   = true;
            } catch(Exception exc) {}
          }
        }
        else
        {
          continue;
        }

        boolean contourCompleted = false;
        int l;
        int numIteractions = 0;
        while (!contourCompleted)
        {
          do
          {
            try
            {
              //System.out.println("Riferimento ["+(ri)+","+(rj)+"]");
              switch (edgeCase)
              {
                case CASE_A:
                  //System.out.println("CASE_A");
                  xNear = xNear_A;
                  yNear = yNear_A;
                  rPoint = rPoint_A;
                  succCase = succCase_A;
                  xflag[ri][rj] = true;
                 break;
                case CASE_B:
                  //System.out.println("CASE_B");
                  xNear = xNear_B;
                  yNear = yNear_B;
                  rPoint = rPoint_B;
                  succCase = succCase_B;
                  break;
                case CASE_C:
                  //System.out.println("CASE_C");
                  xNear = xNear_C;
                  yNear = yNear_C;
                  rPoint = rPoint_C;
                  succCase = succCase_C;
                  break;
                case CASE_D:
                  //System.out.println("CASE_D");
                  xNear = xNear_D;
                  yNear = yNear_D;
                  rPoint = rPoint_D;
                  succCase = succCase_D;
                  xflag[ri][rj - 1] = true;
                  break;
              }

              int rri = 0;
              int rrj = 0;
              for (l = 0; l < 3; l++)
              {

                rri = ri + xNear[l];
                rrj = rj + yNear[l];

                x1 = x[rri];
                y1 = y[rrj];
                z1 = z[rri][rrj];

                int rrii = ri + xNear[l + 1];
                int rrjj = rj + yNear[l + 1];

                x2 = x[rrii];
                y2 = y[rrjj];
                z2 = z[rrii][rrjj];

//System.out.print("["+(ri + xNear[l])+","+(rj + yNear[l])+"] "+" ["+(ri + xNear[l+1])+","+(rj + yNear[l+1])+"] " + l);
                if ( checkIntersection(level, z1, z2) )
                {
                  if(equalZ2)
                  {
                    try
                    {
                        xflag[rrii][rrjj-1] = true;
                        xflag[rrii][rrjj]   = true;
                    } catch(Exception exc) {}
                  }

                  c1 = (level - z1) / (z2 - z1);
                  xc = x1 + (x2 - x1) * c1;
                  yc = y1 + (y2 - y1) * c1;
                  contour.addElement(( currCPoint = new Point2D.Double(xc, yc) ) );
                  ri += xNear[rPoint[l]];
                  rj += yNear[rPoint[l]];
                  edgeCase = succCase[l];
                  break;
                }
              }
              if (l == 3)
              {
                System.out.println("Errore creazione curva di livello");
                currCPoint = firstCPoint;
              }
            }
            catch (Exception exc)
            {

              if (! (exc instanceof IOException))
              {

 //               System.out.println("Eccezzione");

                //Quando una curva di livello esce dalla griglia
                //si verifica una eccezione che gestisco andando
                //alla ricerca sul bordo dove rientra la curva
                //e riprendendo quindi la ricerca dei punti
                //di contour

                boolean found = false;
                int xi, yj;
                int border;

                for(border = 0; border < 4 && !found; border++)
                {

                 switch (edgeCase)
                  {
                    case CASE_B:
                      yj = y.length - 1;
                      for (xi = ri; xi > 0; xi--)
                      {
                        x2 = x[xi];
                        y2 = y[yj];
                        z2 = z[xi][yj];

                        x1 = x[xi - 1];
                        y1 = y[yj];
                        z1 = z[xi - 1][yj];

                        if ( checkIntersection(level, z1, z2) )
                        {
                          found = true;
                          ri = xi - 1;
                          rj = yj;
                          edgeCase = CASE_C;
                          //System.out.println("CASE B Trovata int succ CASE C");
                          break;
                        }
                      }

                      //Non ho trovato nessun punto sul lato
                      //superiore devo cercare un punto sul
                      //bordo laterale CASE_A devo partire dal primo
                      //punto a differenza del caso generico in cui
                      //devo partire dal punto successivo al segmento in cui e'
                      //stato individuato il punto di uscita della curva di livello
                      //in esame, valgono considerazioni analoghe per gli altri casi.
                      if (!found)
                      {
                        //System.out.println("CASE B NON Trovata int succ CASE A");
                        edgeCase = CASE_D;
                        rj = y.length - 1;
                      }
                      break;
                    case CASE_A:
                      xi = x.length - 1;
                      for (yj = rj + 1; yj < y.length - 1; yj++)
                      {
                        x1 = x[xi];
                        y1 = y[yj];
                        z1 = z[xi][yj];

                        x2 = x[xi];
                        y2 = y[yj + 1];
                        z2 = z[xi][yj + 1];

                        xflag[xi][yj] = true;

                        if ( checkIntersection(level, z1, z2) )
                        {
                          found = true;
                          ri = xi;
                          rj = yj + 1;
                          edgeCase = CASE_D;
                          //System.out.println("CASE A Trovata int succ CASE D");
                          break;
                        }
                      }
                      if (!found)
                      {
                        //System.out.println("CASE A NON Trovata int succ CASE C");
                        edgeCase = CASE_B;
                        ri = x.length - 1;
                      }
                      break;
                    case CASE_C:
                      yj = 0;
//                      for (xi = ri - 1; xi >= 0; xi--)
                      for (xi = ri + 1; xi < x.length - 1; xi++)
                      {
                        x1 = x[xi];
                        y1 = y[yj];
                        z1 = z[xi][yj];

                        x2 = x[xi + 1];
                        y2 = y[yj];
                        z2 = z[xi + 1][yj];

                        if ( checkIntersection(level, z1, z2) )
                        {
                          found = true;
                          ri = xi;
                          rj = yj;
                          edgeCase = CASE_B;
                          //System.out.println("CASE C Trovata int succ CASE B");
                          break;
                        }
                      }
                      if (!found)
                      {
                        //System.out.println("CASE C NON Trovata int succ CASE D");
                        edgeCase = CASE_A;
                        rj = -1;
                      }
                      break;
                    case CASE_D:
                      xi = 0;
                      for (yj = rj - 1; yj > 0; yj--)
                      {
                        x1 = x[xi];
                        y1 = y[yj];
                        z1 = z[xi][yj];

                        x2 = x[xi];
                        y2 = y[yj - 1];
                        z2 = z[xi][yj - 1];

                        xflag[xi][yj] = true;

                        if (checkIntersection(level, z1, z2))
                        {
                          found = true;
                          ri = xi;
                          rj = yj - 1;
                          edgeCase = CASE_A;
                          //System.out.println("CASE D Trovata int succ CASE A");
                          break;
                        }
                      }
                      if (!found)
                      {
                        //System.out.println("CASE D NON Trovata int succ CASE B");
                        edgeCase = CASE_C;
                        ri = -1;
                      }
                      break;
                  }
                }
                /*
                 * Per gestire correttamente le curve di livello che escono
                 * dalla griglia come curve spezzate devo memorizzare ogni
                 * singola spezzata separatamente per evitare che in fase
                 * di plot vengano congiunti con un segmento i punti di
                 * uscita dalla griglia della curva di livello in esame.
                 */
                if (contour.size() >= 2)
                {
                  contours.addElement(contour);
                  contour = new Vector<Point2D.Double>();
                }
                else
                {
                  contour.clear();
                }

                c1 = (level - z1) / (z2 - z1);
                xc = x1 + (x2 - x1) * c1;
                yc = y1 + (y2 - y1) * c1;
                contour.addElement( ( currCPoint = new Point2D.Double(xc, yc) ) );

                if(!found && border == 4)
                {
//                  System.out.println("Completato il bordo");
                  numIteractions = maxIteractions;
                }
              }
            }
            //System.out.println("Edge case" + edgeCase );
            numIteractions++;
            if(numIteractions > maxIteractions)
              break;
          }

          /* La curva di livello si ritiene conclusa quando
             si ritorna al punto di partenza
           */
          while ( !( currCPoint.equals(firstCPoint) ) ) ;

/*
          if (numIteractions > maxIteractions)
            System.out.println("Raggiunto numero massimo di iterazioni");
*/
          if (contour.size() >= 2)
          {
            contours.addElement(contour);
            contour = new Vector<Point2D.Double>();
          }
          else
          {
            contour.clear();

          }
          //if(true) return contours;
          contourCompleted = true;
        }
      }
    }
    xflag = null;
    return contours;
  }

  public void setMinMax(float xmin, float xmax,
                        float ymin, float ymax,
                        float zmin, float zmax)
  {
    setMinMaxX(xmin, xmax);
    setMinMaxY(ymin, ymax);
    setMinMaxZ(zmin, zmax);
  }

  public void setMinMaxX(double xmin, double xmax)
  {
    this.xmin = xmin;
    this.xmax = xmax;
  }

  public void setMinMaxY(double ymin, double ymax)
  {
    this.ymin = ymin;
    this.ymax = ymax;
  }

  public void setMinMaxZ(double zmin, double zmax)
  {
    this.zmin = zmin;
    this.zmax = zmax;
  }

  private void computeMinMax()
  {
    xmin = xmax = x[0];
    ymin = ymax = y[0];
    zmin = zmax = z[0][0];

    for (int i = 0; i < x.length; i++)
    {
      if (x[i] < xmin)
      {
        xmin = x[i];
      }
      if (x[i] > xmax)
      {
        xmax = x[i];
      }
      for (int j = 0; j < z[0].length; j++)
      {
        if (z[i][j] < zmin)
        {
          zmin = z[i][j];
        }
        if (z[i][j] > zmax)
        {
          zmax = z[i][j];
        }
      }
    }

    for (int i = 0; i < y.length; i++)
    {
      if (y[i] < ymin)
      {
        ymin = y[i];
      }
      if (y[i] > ymax)
      {
        ymax = y[i];
      }
    }

    equalCases();
  }

  private void equalCases()
  {
    if (xmax == xmin)
    {
      xmin -= xmax / 10.f;
      xmax += xmax / 10.f;
    }
    if (ymax == ymin)
    {
      ymin -= ymax / 10.f;
      ymax += ymax / 10.f;
    }
    if (zmax == zmin)
    {
      zmin -= zmax / 10.f;
      zmax += zmax / 10.f;
    }
  }
}
