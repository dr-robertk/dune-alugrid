// (c) Robert Kloefkorn 2010 

#include "mapp_tetra_3d.h"
#include "gitter_tetra_top.h"

// #     #                                  #####  #######
// #     #  ######    ##     ####   ###### #     #    #      ####   #####
// #     #  #        #  #   #    #  #            #    #     #    #  #    #
// #######  #####   #    #  #       #####   #####     #     #    #  #    #
// #     #  #       ######  #       #            #    #     #    #  #####
// #     #  #       #    #  #    #  #      #     #    #     #    #  #
// #     #  #       #    #   ####   ######  #####     #      ####   #


template < class A > void Hface3Top < A > :: split_e01 () 
{
  int l = 1 + level () ;
  myvertex_t * ev0 = this->myhedge1(1)->subvertex (2) ;
  assert(ev0) ;
  inneredge_t * e0 = new inneredge_t (l, ev0, this->myvertex (2), getEdgeIndexManager() ) ;
  assert( e0 ) ;
  innerface_t * f0 = new innerface_t (l, this->myhedge1(2), this->twist(2), this->subedge1(0,0), this->twist(0) , e0, 0, _indexManager, 0) ;
  innerface_t * f1 = new innerface_t (l, e0, 1, this->subedge1(0,1), this->twist(0), this->myhedge1(1),  this->twist(1), _indexManager, 1) ;
  assert (f0 && f1 ) ;
  f0->append(f1) ;
  _ed  = e0 ;
  _dwn = f0 ;
  _rule = myrule_t :: e01 ;
  return ;
}

template < class A >  void Hface3Top < A > :: split_e12 () 
{
  int l = 1 + level () ;
  myvertex_t * ev0 = this->myhedge1(1)->subvertex (0) ;
  assert(ev0) ;
  inneredge_t * e0 = new inneredge_t (l, ev0, this->myvertex (0), getEdgeIndexManager()) ;
  assert( e0 ) ;
  innerface_t * f0 = new innerface_t (l, this->myhedge1(0), this->twist(0), this->subedge1(1,0), this->twist(1) , e0, 0, _indexManager, 0 ) ;
  innerface_t * f1 = new innerface_t (l, e0, 1, this->subedge1(1,1), this->twist(1), this->myhedge1(2),  this->twist(2), _indexManager, 1 ) ;
  assert (f0 && f1 ) ;
  f0->append(f1) ;
  _ed  = e0 ;
  _dwn = f0 ;
  _rule = myrule_t :: e12 ;
  return ;
}

template < class A >  void Hface3Top < A > :: split_e20 () 
{
  int l = 1 + level () ;
  myvertex_t * ev0 = this->myhedge1(1)->subvertex (1) ;
  assert(ev0) ;
  inneredge_t * e0 = new inneredge_t (l, ev0, this->myvertex(1), getEdgeIndexManager()) ;
  assert( e0 ) ;
  innerface_t * f0 = new innerface_t (l, this->myhedge1(1), this->twist(1), this->subedge1(2,0), this->twist(2) , e0, 0, _indexManager, 0 ) ;
  innerface_t * f1 = new innerface_t (l, e0, 1, this->subedge1(2,1), this->twist(2), this->myhedge1(0),  this->twist(0), _indexManager, 1 ) ;
  assert (f0 && f1 ) ;
  f0->append(f1) ;
  _ed  = e0 ;
  _dwn = f0 ;
  _rule = myrule_t :: e20 ;
  return ;
}

template < class A >  void Hface3Top < A > :: split_iso4 () 
{
  int l = 1 + level () ;
  myvertex_t * ev0 = this->myhedge1(0)->subvertex (0) ;
  myvertex_t * ev1 = this->myhedge1(1)->subvertex (0) ;
  myvertex_t * ev2 = this->myhedge1(2)->subvertex (0) ;
  assert(ev0 && ev1 && ev2 ) ;
  IndexManagerType & im = getEdgeIndexManager();
  inneredge_t * e0 = new inneredge_t (l, ev0, ev1, im) ;
  inneredge_t * e1 = new inneredge_t (l, ev1, ev2, im) ;
  inneredge_t * e2 = new inneredge_t (l, ev2, ev0, im) ;
  assert( e0 && e1 && e2 ) ;
  e0->append(e1) ;
  e1->append(e2) ;
  innerface_t * f0 = new innerface_t (l, this->subedge1(0,0), this->twist(0), e2, 1, this->subedge1(2,1), this->twist(2), _indexManager, 0) ;
  innerface_t * f1 = new innerface_t (l, this->subedge1(0,1), this->twist(0), this->subedge1(1,0), this->twist(1), e0, 1, _indexManager, 1) ;
  innerface_t * f2 = new innerface_t (l, e1, 1, this->subedge1(1,1), this->twist(1), this->subedge1(2,0), this->twist(2), _indexManager, 2) ;
  innerface_t * f3 = new innerface_t (l, e0, 0, e1, 0, e2, 0, _indexManager, 3 ) ;
  assert (f0 && f1 && f2 && f3) ;
  f0->append(f1) ;
  f1->append(f2) ;
  f2->append(f3) ;
  _ed  = e0 ;
  _dwn = f0 ;
  _rule = myrule_t :: iso4 ;
  return ;
}

template < class A > void Hface3Top < A > :: refineImmediate (myrule_t r) 
{
  if (r != getrule ()) {
    assert (getrule () == myrule_t :: nosplit) ;
    switch(r) {
      typedef typename myhedge1_t :: myrule_t myhedge1rule_t;
      case myrule_t :: e01 :
        this->myhedge1 (0)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (0))) ;
  split_e01 () ;
  break ;
      case myrule_t :: e12 :
        this->myhedge1 (1)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (0))) ;
  split_e12 () ;
  break ;
      case myrule_t :: e20 :
        this->myhedge1 (2)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (0))) ;
  split_e20 () ;
  break ;
      case myrule_t :: iso4 :
        this->myhedge1 (0)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (0))) ;
        this->myhedge1 (1)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (1))) ;
        this->myhedge1 (2)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (2))) ;
        split_iso4 () ;
        break ;
      default :
        cerr << "**FEHLER (FATAL) falsche Verfeinerungsregel [" << r << "] in " << __FILE__ << " " << __LINE__ << endl ;
        abort () ;
        break ;
    }

    // set parent rule 
    {
      myrule_t myRule = getrule();
      for (innerface_t * f = down () ; f ; f = f->next ()) 
      {
        f->_parRule = myRule;
      }
    }
    this->postRefinement () ;
  }
  return ;
}

template < class A > bool Hface3Top < A > :: refine (myrule_t r, int twist) 
{
  if (r != getrule ()) {
    assert (getrule () == myrule_t :: nosplit ? 1 : 
      (cerr << "**FEHLER beim Verfeinern mit Regel " << r << " auf " << getrule () << endl, 0)) ;
    switch(r) {
      case myrule_t :: e01 :
      case myrule_t :: e12 :
      case myrule_t :: e20 :
      case myrule_t :: iso4 :
      {
        bool a = (twist < 0) 
               ? this->nb.front ().first->refineBalance (r,this->nb.front ().second)
               : this->nb.rear  ().first->refineBalance (r,this->nb.rear  ().second) ;

        if (a) 
        {  
          if (getrule () == myrule_t :: nosplit) {
            refineImmediate (r) ;
            { 
              for (innerface_t * f = down () ; f ; f = f->next ()) f->nb = this->nb ; 
            }
          } 
          else 
          {
            // Als Test absichern, da"s die Verfeinerung durchgekommen ist. Im
            // anisotropen Fall darf das so nicht mehr gemacht werden.
            assert (getrule () == r) ;
          }

          this->postRefinement () ;
          return true ;
        } 
        else 
        {
          return false ;
        }
      }
      default :
        cerr << "**WARNUNG (IGNORIERT) falsche Verfeinerungsregel gefunden: " ;
        cerr << "[" << r << "] in " << __FILE__ << " " << __LINE__ << endl ;
        return false ;
    }
  }
  return true ;
}

template < class A > bool Hface3Top < A > :: coarse () 
{
  innerface_t * f = down () ;
  if (!f) return false ;
  bool x = true ;
  do {

  // Falls eine Kind-Fl"ache noch referenziert wird, kann
  // nicht auf diesem Level vergr"obert werden.
  // Daher wird nur die nichtkonforme Nachbarschaft ver-
  // vollst"andigt, die eventuell durch Elementvergr"oberung
  // durcheinander gekommen war. Die Vergr"oberung geht dann
  // auf das n"achste Level "uber.
  
    if (f->ref) {
      if (f->ref == 1) f->nb.complete (this->nb) ;
      f->coarse () ;
      x = false ;
    }
  } while ( (f = f->next()) ) ;
  if (x) {
    
  // Hier wird tats"achlich vergr"obert, d.h. alle Kinder 
  // werden beseitigt, und das Bezugsobjekt wird zum neuen
  // Blatt im Baum.
    
    delete _dwn ; 
    _dwn = 0 ;
    delete _ed ;
    _ed = 0 ;
    _rule = myrule_t :: nosplit ;
    {for (int i = 0 ; i < 3 ; i ++ ) this->myhedge1 (i)->coarse () ; }
  }
  return x ;
}

template < class A > void Hface3Top < A > :: backup (ostream & os) const 
{
  doBackup( os );
}

template < class A > void Hface3Top < A > :: backup (ObjectStream & os) const 
{
  doBackup( os );
}

template < class A > 
template < class OutStream_t>
void Hface3Top < A > :: doBackup (OutStream_t & os) const 
{
  os.put ((char) getrule ()) ;
  {for (const inneredge_t * e = innerHedge () ; e ; e = e->next ()) e->backup (os) ; }
  {for (const innerface_t * c = down () ; c ; c = c->next ()) c->backup (os) ; }
  return ;
}

template < class A > void Hface3Top < A > :: restore (istream & is) 
{
  doRestore( is );
}

template < class A > void Hface3Top < A > :: restore (ObjectStream& is) 
{
  doRestore( is );
}

template < class A > template <class InStream_t> 
void Hface3Top < A > :: doRestore (InStream_t & is) 
{
  refineImmediate (myrule_t ((char) is.get ())) ;
  {for (inneredge_t * e = innerHedge () ; e ; e = e->next ()) e->restore (is) ; }
  {for (innerface_t * c = down () ; c ; c = c->next ()) c->restore (is) ; }
  return ;
}

// #     #                          #####  #######
// #     #  #####   #    #  #####  #     #    #      ####   #####
// #     #  #    #  ##   #  #    #       #    #     #    #  #    #
// #######  #####   # #  #  #    #  #####     #     #    #  #    #
// #     #  #    #  #  # #  #    #       #    #     #    #  #####
// #     #  #    #  #   ##  #    # #     #    #     #    #  #
// #     #  #####   #    #  #####   #####     #      ####   #

template < class A >  void Hbnd3Top < A > :: 
setBoundaryId (const int id ) 
{
  myhface3_t & face = *(this->myhface3(0));
  face.setBndId( id );
  // 3 vertices and edges 
  for(int i=0; i<3; ++i) 
  {
    face.myvertex(i)->setBndId( id );
    face.myhedge1(i)->setBndId( id );
  }
}

template < class A >  void Hbnd3Top < A > :: split_e01 () 
{
  int l = 1 + level () ;
  int gFace = this->getGhost().second ;
  innerbndseg_t * b0 = new innerbndseg_t (l, this->subface3 (0,0), this->twist (0), this->_projection, this , _bt, _indexManager, 0 , gFace) ;
  innerbndseg_t * b1 = new innerbndseg_t (l, this->subface3 (0,1), this->twist (0), this->_projection, this , _bt, _indexManager, 0 , gFace) ;
  assert (b0 && b1) ;
  b0->append(b1) ;
  _dwn = b0 ;
  return ;
}

template < class A >  void Hbnd3Top < A > :: split_e12 () {
  int l = 1 + level () ;
  int gFace = this->getGhost().second ;
  innerbndseg_t * b0 = new innerbndseg_t (l, this->subface3 (0,0), this->twist (0), this->_projection, this , _bt, _indexManager, 0 , gFace) ;
  innerbndseg_t * b1 = new innerbndseg_t (l, this->subface3 (0,1), this->twist (0), this->_projection, this , _bt, _indexManager, 0 , gFace) ;
  assert (b0 && b1) ;
  b0->append(b1) ;
  _dwn = b0 ;
  return ;
}

template < class A >  void Hbnd3Top < A > :: split_e20 () 
{
  int l = 1 + level () ;
  int gFace = this->getGhost().second ;
  innerbndseg_t * b0 = new innerbndseg_t (l, this->subface3 (0,0), this->twist (0), this->_projection, this , _bt, _indexManager, 0, gFace) ;
  innerbndseg_t * b1 = new innerbndseg_t (l, this->subface3 (0,1), this->twist (0), this->_projection, this , _bt, _indexManager, 0, gFace) ;
  assert (b0 && b1) ;
  b0->append(b1) ;
  _dwn = b0 ;
  return ;
}

template < class A >  void Hbnd3Top < A > :: split_iso4 () 
{
  int l = 1 + level () ;
 
  typedef typename Gitter :: GhostChildrenInfo GhostChildrenInfo; 
  GhostChildrenInfo ghostInfo;
  // ghostInfo is filled by splitGhost, see gitter_tetra_top_pll.h
  this->splitGhost( ghostInfo );

  innerbndseg_t * b0 = new innerbndseg_t (l, this->subface3 (0,0), this->twist (0), this->_projection, this , _bt, _indexManager, ghostInfo.child(0), ghostInfo.face(0)) ;
  innerbndseg_t * b1 = new innerbndseg_t (l, this->subface3 (0,1), this->twist (0), this->_projection, this , _bt, _indexManager, ghostInfo.child(1), ghostInfo.face(1)) ;
  innerbndseg_t * b2 = new innerbndseg_t (l, this->subface3 (0,2), this->twist (0), this->_projection, this , _bt, _indexManager, ghostInfo.child(2), ghostInfo.face(2)) ;
  innerbndseg_t * b3 = new innerbndseg_t (l, this->subface3 (0,3), this->twist (0), this->_projection, this , _bt, _indexManager, ghostInfo.child(3), ghostInfo.face(3)) ;
  assert (b0 && b1 && b2 && b3) ;
  b0->append(b1) ;
  b1->append(b2) ;
  b2->append(b3) ;
  _dwn = b0 ;
  
  return ;
}

template < class A >  bool Hbnd3Top < A > :: coarse () 
{
  innerbndseg_t * b = down () ;
  if (!b) return false ;
  bool x = true ;
  do {
    if( (b->myhface3(0)->ref > 1) ) ((b->coarse ()), x = false) ;
  } while ( (b = b->next()) ) ;
  if (x) 
  {
    if (!this->lockedAgainstCoarsening ()) 
    {
      this->preCoarsening () ;
      
      delete _dwn ; _dwn = 0 ;
      this->myhface3 (0)->coarse () ;

      this->coarseGhost();
    }
  }
  return x ;
}

template < class A >  bool Hbnd3Top < A > :: bndNotifyCoarsen () {
  return coarse () ;
}

template < class A >  bool Hbnd3Top < A > :: refineBalance (balrule_t r, int b) 
{

  // Die Methode refineBalance () f"uhrt auf dem Randabschluss entweder
  // unbedingt die Verfeinerung durch, da im Verlauf der Verfeinerung keine
  // weiteren Anforerungen mehr an den Randabschluss  gerichtet werden 
  // ODER gibt die Verfeinerung als nicht erf"ullt zur"uck: Dann liegt
  // es am Aufrufer die Verfeinerung nochmals anzuforern.

  assert (b == 0) ;
  assert (this->leaf ()) ;
  if (!bndNotifyBalance (r,b)) 
  {
  
    // Hier kann der innere Rand [parallel] die Verfeinerung
    // verhindern, damit z.B. das Durchverfeinern im anisotropen
    // Fall erstmal nicht stattfindet, wenn nicht klar ist, wie die
    // weitere Rekursion aussieht. Dazu muss auf dem Niveau der Klasse
    // des Template-Arguments die Methode bndNotifyBalance () "uber-
    // schrieben werden. Die Defaultmethode liefert immer 'true'.
  
    return false ;
  } 
  else {
    if(r == myrule_t :: iso4) 
    {
      // Der Rand verfeinert unbedingt die anliegende Fl"ache und dann
      // sich selbst, weil die Anforderung durch die Fl"ache kam, und
      // dahinter keine Balancierung stattfinden muss.
    
      this->myhface3 (0)->refineImmediate (r) ;
      split_iso4 () ;
    } 
    else if (r == myrule_t :: e01) {
      this->myhface3 (0)->refineImmediate (r) ;
      split_e01 () ;
    } 
    else if (r == myrule_t :: e12) {
      this->myhface3 (0)->refineImmediate (r) ;
      split_e12 () ;
    } 
    else if (r == myrule_t :: e20) {
      this->myhface3 (0)->refineImmediate (r) ;
      split_e20 () ;
    } 
    else {
      cerr << "**FEHLER (FATAL, weil nicht vorgesehen) beim Verfeinern am " ;
      cerr << "Randst\"uck mit der Regel [" << r << "] in " ;
      cerr << __FILE__ << " " << __LINE__ << endl ;
      abort () ;
    }
    
    // postRefinement () gibt die M"oglichkeit auf dem Niveau des
    // Template-Arguments eine Methode aufzurufen, um eventuelle
    // Operationen auf dem verfeinerten Randst"uck durchzuf"uhren.
    
    this->postRefinement () ;
    return true ;
  }
}

template < class A >  bool Hbnd3Top < A > :: refineLikeElement (balrule_t r) 
{

  // Mit der Methode refineLikeElement () verh"alt sich ein Randabschluss
  // in der Verfeinerung wie ein Element: Es wird zuerst gepr"uft ob eine
  // Balancierung der Vererfeinerung durch die Fl"ache hindurch erfolgreich
  // ist und nur genau dann die Verfeinerung durchgef"uhrt mit R"uckgabewert
  // 'true'. Diese Methode bedient eigentlich nur die parallele Verfeinerung
  // kann aber auch auf jedem beliebigen Randelement im seriellen Fall auf-
  // gerufen werden ohne Schaden anzurichten: Eine 1-Level Verfeinerung am
  // Rand ist jedoch wirkungslos, da sie beim n"achsten Vergr"obern wieder
  // aufgel"ost ist. Erst die mehrfache Anwendung f"uhrt durch die 
  // Balancierung zu einer "Anderung am Elementgitter.

  if (r == myrule_t :: nosplit) {
  
    cerr << "**WARNUNG (IGNORIERT) beim Versuch mit nosplit zu Verfeinern" ;
    cerr << "  in " << __FILE__ << " " << __LINE__ << endl ;
    
      // Eine Anforderung mit nosplit zu Verfeinern nur erf"ullt,
  // falls die zugeh"orige Fl"achenregel auch nosplit ist, sonst
  // wird die Anforderung als nicht erf"ullt zur"uckgegeben.
    
    return this->getrule () == balrule_t :: nosplit ? true : false ;
    
  } else {
    if (this->getrule () == r) {
    
      // Alles schon wie es sein soll -> true.
    
      return true ;
    } else {
  
  // Der nachfolgende Test bezieht sich auf die Verfeinerungssituation
  // der Fl"ache, da getrule () auf myhface3 (0)->getrule () umgeleitet
  // ist.
  
      assert (this->getrule () == myrule_t :: nosplit) ;
      switch (r) {
      case balrule_t :: e01 :
        if (!this->myhface3 (0)->refine (balrule_t (balrule_t :: e01).rotate (this->twist (0)), this->twist (0))) return false ;
        split_e01 () ;
        break;
      case balrule_t :: e12 :
        if (!this->myhface3 (0)->refine (balrule_t (balrule_t :: e12).rotate (this->twist (0)), this->twist (0))) return false ;
        split_e12 () ;
        break;
      case balrule_t :: e20 :
        if (!this->myhface3 (0)->refine (balrule_t (balrule_t :: e20).rotate (this->twist (0)), this->twist (0))) return false ;
        split_e20 () ;
        break;
      case balrule_t :: iso4 :
        if (!this->myhface3 (0)->refine (balrule_t (balrule_t :: iso4).rotate (this->twist (0)), this->twist (0))) return false ;
        split_iso4 () ;
        break;
      default :
        cerr << "**WARNUNG (FEHLER IGNORIERT) falsche Verfeinerungsregel [" << this->getrule () ;
        cerr << "] (ignoriert) in " << __FILE__ << " " << __LINE__ << endl ;
        return false ;
      }

      // postRefinement () gibt die M"oglichkeit auf dem Niveau des
  // Template-Arguments eine Methode aufzurufen, um eventuelle
  // Operationen auf dem verfeinerten Randst"uck durchzuf"uhren.
      this->postRefinement () ;
      return true ;
    }
  }
}

template < class A >  void Hbnd3Top < A > :: restoreFollowFace () 
{
  // retoreFollowFace () veranlasst das Randelement sich am 
  // bestehenden Fl"achenbaum wiederherzustellen durch die
  // entsprechende Verfeinerung.
  
  myhface3_t & f (*(this->myhface3 (0))) ;
  if (!f.leaf ()) {
    balrule_t r = f.getrule () ;
    switch (r) {
      case myrule_t :: e01 :
        split_e01 () ;
        break ;
      case myrule_t :: e12 :
        split_e12 () ;
        break ;
      case myrule_t :: e20 :
        split_e20 () ;
        break ;
      case myrule_t :: iso4 :
        split_iso4 () ;
        break ;
      default :
        cerr << "**FEHLER (FATAL) beim Verfeinern am Randst\"uck mit der Regel [" << r << "] in " << __FILE__ << " " << __LINE__ << endl ;
        abort () ;
        break ;
    }
    this->postRefinement () ;
    {for (innerbndseg_t * b = down () ; b ; b = b->next ()) b->restoreFollowFace () ; }
  }
  return ;
}

// #######                                 #######
//    #     ######   #####  #####     ##      #      ####   #####
//    #     #          #    #    #   #  #     #     #    #  #    #
//    #     #####      #    #    #  #    #    #     #    #  #    #
//    #     #          #    #####   ######    #     #    #  #####
//    #     #          #    #   #   #    #    #     #    #  #
//    #     ######     #    #    #  #    #    #      ####   #

template < class A > TetraTop < A > 
:: TetraTop (int l, myhface3_t * f0, int t0,
             myhface3_t * f1, int t1, myhface3_t * f2, int t2, 
             myhface3_t * f3, int t3, innertetra_t *up, int nChild, double vol) 
  : A (f0, t0, f1, t1, f2, t2, f3, t3, up->_myGrid), _dwn (0), _bbb (0), _up(up), _fc (0), _ed (0)
  , _indexManager(up->_indexManager) 
  , _volume( (vol < 0.0) ?  
            quadraturTetra3D < VolumeCalc > (
                LinearMapping ( this->myvertex(0)->Point(), 
                                this->myvertex(1)->Point(),
                                this->myvertex(2)->Point(), 
                                this->myvertex(3)->Point())).integrate1 (0.0) 
            : vol )
  , _lvl (l) 
  , _rule (myrule_t :: nosplit)
  , _nChild(nChild) 
{
  // set level 
  assert( this->level() == l );
  
  // _up wird im Constructor uebergeben
  this->setIndex( _indexManager.getIndex() );

  // we need boundary id now, for elements is the same as fathers 
  this->_bndid = _up->bndId();

  // check that volume is calculated correctly 
  assert( 
      fabs( 
    quadraturTetra3D < VolumeCalc > (
      LinearMapping ( this->myvertex(0)->Point(), 
                      this->myvertex(1)->Point(),
                      this->myvertex(2)->Point(), 
                      this->myvertex(3)->Point())).integrate1 (0.0)  - _volume ) < 1e-10 );
  return ;
}

// constrcutor mit IndexManager uebergabe
// this is the macro element constructor 
template < class A > TetraTop < A > :: 
TetraTop (int l, myhface3_t * f0, int t0,
          myhface3_t * f1, int t1, myhface3_t * f2, int t2, 
          myhface3_t * f3, int t3, IndexManagerType & im, Gitter * mygrid) 
  : A (f0, t0, f1, t1, f2, t2, f3, t3, mygrid),
    _dwn (0), _bbb (0), _up(0), _fc (0),_ed (0)
  , _indexManager(im)
  , _volume( quadraturTetra3D < VolumeCalc > 
    (LinearMapping ( this->myvertex(0)->Point(), this->myvertex(1)->Point(),
                     this->myvertex(2)->Point(), this->myvertex(3)->Point())).integrate1 (0.0) )
  , _lvl (l) 
  , _rule (myrule_t :: nosplit) 
  , _nChild(0)  // we are macro ==> nChild 0 
{ 
  assert( this->level() == l );

  // _up wird im Constructor uebergeben
  this->setIndex( _indexManager.getIndex() );
  return ;
}

template < class A > TetraTop < A > :: ~TetraTop () 
{
  this->freeIndex( this->_indexManager );
  // attachleafs is called in constructor of TetraEmpty
  // if delete is called on macro we only call this method on leaf
  if (!_dwn ) this->detachleafs();
  if (_bbb) delete _bbb ;
  if (_dwn) delete _dwn ;
  if (_fc) delete _fc ;
  if (_ed) delete _ed ;
  return ;
}

template < class A >  typename TetraTop < A > :: myhedge1_t * TetraTop < A > :: subedge1 (int i, int j) {
  switch (this->myhface3(i)->getrule()) {
  case myhface3_t :: myrule_t :: e01 :
    assert( j == 0 );
    return this->myhface3 (i)->subedge1 (0) ;
  case myhface3_t :: myrule_t :: e12 :
    assert( j == 0 );
    return this->myhface3 (i)->subedge1 (0) ;
  case myhface3_t :: myrule_t :: e20 :
    assert( j == 0 );
    return this->myhface3 (i)->subedge1 (0) ;
  case myhface3_t :: myrule_t :: iso4 :
    assert( j < 3 );
    return ((this->twist (i) < 0) ? this->myhface3 (i)->subedge1 ((8 - j + this->twist (i)) % 3) : this->myhface3 (i)->subedge1 ((j + this->twist (i)) % 3)) ;
  case myhface3_t :: myrule_t :: nosplit :
    cerr << "**FEHLER (FATAL): subedge1 () auf nicht in verfeinerter Fl\"ache aufgerufen. In " << __FILE__ << " " << __LINE__ << endl ;
    abort () ;
    return 0 ;
  }
  return 0 ;
}

template < class A >  const typename TetraTop < A > :: myhedge1_t * TetraTop < A > :: subedge1 (int i, int j) const {
  return ((TetraTop < A > *)this)->subedge1 (i,j) ;
}


template < class A >  typename TetraTop < A > ::  myhface3_t * TetraTop < A > :: subface3 (int i, int j) {
  switch (this->myhface3(i)->getrule()) {
  case myhface3_t :: myrule_t :: e01 :
    assert( j < 2 );
    if ( this->twist(i) == 0 ||  this->twist(i) == 1 ||  this->twist(i) == -1 )
      return this->myhface3(i)->subface3(j) ;
    if ( this->twist(i) == 2 ||  this->twist(i) == -2 || this->twist(i) == -3 )
      return this->myhface3(i)->subface3(!j) ;
      cerr << __FILE__ << " " << __LINE__ << "myhface3(i)->subface3()" << endl;
      return 0;
  case myhface3_t :: myrule_t :: e12 :
    assert( j < 2 );
    if ( this->twist(i) == 0 ||  this->twist(i) == 2 ||  this->twist(i) == -3 )
      return this->myhface3(i)->subface3(j) ;
    if ( this->twist(i) == -1 || this->twist(i) == 1 ||  this->twist(i) == -2 )
      return this->myhface3(i)->subface3(!j) ;
    cerr << __FILE__ << " " << __LINE__ << "myhface3(i)->subface3()" << endl;
    return 0;
  case myhface3_t :: myrule_t :: e20 :
    assert( j < 2 );
    if ( this->twist(i) == 1 ||  this->twist(i) == 2 ||  this->twist(i) == -2 )
      return this->myhface3(i)->subface3(j) ;
    if ( this->twist(i) == 0 ||  this->twist(i) == -1 || this->twist(i) == -3 )
      return this->myhface3(i)->subface3(!j) ;
    cerr << __FILE__ << " " << __LINE__ << "myhface3(i)->subface3()" << endl;
    return 0;
  case myhface3_t :: myrule_t :: iso4 :
    assert( j < 4 );
    if ( j == 3 )
      return this->myhface3(i)->subface3(3);
    if ( j < 3 )
      return this->myhface3(i)->subface3(this->twist(i) < 0 ? (7 - j + this->twist(i)) % 3 : (j + this->twist(i)) % 3) ;
  case myhface3_t :: myrule_t :: nosplit :
    cerr << "**FEHLER (FATAL): subface3 () auf nicht verfeinerter Fl\"ache aufgerufen. In " << __FILE__ << " " << __LINE__ << endl ;
    abort () ;
    return 0 ;
  default:
    cerr << "**FEHLER (FATAL): Falsche Verfeinerungsregel [" << this->myhface3(i)->getrule() << "] in "__FILE__ << " " << __LINE__ << endl ;
    abort() ;
  } 
  return 0 ;
}

template < class A >  const typename TetraTop < A > ::  myhface3_t * TetraTop < A > :: subface3 (int i, int j) const {
  return ((TetraTop < A > *)this)->subface3 (i,j) ;
}

template < class A >  IndexManagerType &  TetraTop < A > :: getFaceIndexManager () {
   return static_cast<innerface_t &> (*(static_cast<TetraTop < A > *> (this)->subface3(0,0))).getIndexManager();
}

template < class A >  IndexManagerType &  TetraTop < A > :: getEdgeIndexManager () {
   return static_cast<inneredge_t &> (*(static_cast<TetraTop < A > *> (this)->subedge1(0,0))).getIndexManager();
}

template < class A >  void TetraTop < A > :: split_e01 () {
  int l = 1 + this->level () ;
  
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 3), 1, this->subedge1 (0, 3), 0, this->subedge1 (2, 2), 0, getFaceIndexManager() ) ;
  assert(f0) ;

  // we divide by 2 means we divide the volume by 2
  const double childVolume = calculateChildVolume( 0.5 * _volume );
  
  innertetra_t * h0 = new innertetra_t (l, this->subface3(0, 0), this->twist (0), f0, 0, this->myhface3(2), this->twist (2), this->subface3(3, 0), this->twist (3), this, 0, childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 1), this->twist (0), this->myhface3(1), this->twist (1), f0, 1, this->subface3(3, 1), this->twist (3), this, 1, childVolume) ;
  assert(h0 && h1) ;
  h0->append(h1) ;
  _fc = f0 ;
  _dwn = h0 ;
  h0->_up = h1->_up = this; 
  return ;
}

template < class A >  void TetraTop < A > :: split_e12 () {
  int l = 1 + this->level () ;
  
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 3), 1, this->subedge1 (0, 3), 0, this->subedge1 (2, 2), 0, getFaceIndexManager() ) ;
  assert(f0 ) ;

  // we divide by 2 means we divide the volume by 2
  const double childVolume = calculateChildVolume( 0.5 * _volume );
  
  innertetra_t * h0 = new innertetra_t (l, this->subface3(0, 0), this->twist (0), f0, 0, this->myhface3(2), this->twist (2), this->subface3(3, 0), this->twist (3), this, 0, childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 1), this->twist (0), this->myhface3(1), this->twist (1), f0, 1, this->subface3(3, 1), this->twist (3), this, 1, childVolume) ;
  assert(h0 && h1) ;
  h0->append(h1) ;
  _fc = f0 ;
  _dwn = h0 ;
  _rule = myrule_t :: e12 ;
  return ;
}

template < class A >  void TetraTop < A > :: split_e20 () {
  int l = 1 + this->level () ;
  
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 3), 1, this->subedge1 (0, 3), 0, this->subedge1 (2, 2), 0, getFaceIndexManager() ) ;
  assert(f0) ;

  // we divide by 2 means we divide the volume by 2
  const double childVolume = calculateChildVolume( 0.5 * _volume );
  
  innertetra_t * h0 = new innertetra_t (l, this->subface3(0, 0), this->twist (0), f0, 0, this->myhface3(2), this->twist (2), this->subface3(3, 0), this->twist (3), this, 0, childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 1), this->twist (0), this->myhface3(1), this->twist (1), f0, 1, this->subface3(3, 1), this->twist (3), this, 1, childVolume) ;
  assert(h0 && h1) ;
  h0->append(h1) ;
  _fc = f0 ;
  _dwn = h0 ;
  _rule = myrule_t :: e20 ;
  return ;
}

template < class A >  void TetraTop < A > :: split_e23 () {
  int l = 1 + this->level () ;
  
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 3), 1, this->subedge1 (0, 3), 0, this->subedge1 (2, 2), 0, getFaceIndexManager() ) ;
  assert(f0) ;

  // we divide by 2 means we divide the volume by 2
  const double childVolume = calculateChildVolume( 0.5 * _volume );
  
  innertetra_t * h0 = new innertetra_t (l, this->subface3(0, 0), this->twist (0), f0, 0, this->myhface3(2), this->twist (2), this->subface3(3, 0), this->twist (3), this, 0, childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 1), this->twist (0), this->myhface3(1), this->twist (1), f0, 1, this->subface3(3, 1), this->twist (3), this, 1, childVolume) ;
  assert(h0 && h1) ;
  h0->append(h1) ;
  _fc = f0 ;
  _dwn = h0 ;
  _rule = myrule_t :: e23 ;
  return ;
}

template < class A >  void TetraTop < A > :: split_e30 () {
  int l = 1 + this->level () ;
  
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 3), 1, this->subedge1 (0, 3), 0, this->subedge1 (2, 2), 0, getFaceIndexManager() ) ;
  assert(f0) ;
  double childVolume = 0.5 * _volume;
  innertetra_t * h0 = new innertetra_t (l, this->subface3(0, 0), this->twist (0), f0, 0, this->myhface3(2), this->twist (2), this->subface3(3, 0), this->twist (3), this, 0, childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 1), this->twist (0), this->myhface3(1), this->twist (1), f0, 1, this->subface3(3, 1), this->twist (3), this, 1, childVolume) ;
  assert(h0 && h1) ;
  h0->append(h1) ;
  _fc = f0 ;
  _dwn = h0 ;
  _rule = myrule_t :: e30 ;
  return ;
}

template < class A >  void TetraTop < A > :: split_e31 () 
{
  int l = 1 + this->level () ;
  
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 3), 1, this->subedge1 (0, 3), 0, this->subedge1 (2, 2), 0, getFaceIndexManager()) ;
  assert(f0) ;

  // we divide by 2 means we divide the volume by 2
  const double childVolume = calculateChildVolume( 0.5 * _volume );
  
  innertetra_t * h0 = new innertetra_t (l, this->subface3(0, 0), this->twist (0), f0, 0, this->myhface3(2), this->twist (2), this->subface3(3, 0), this->twist (3), this, 0, childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 1), this->twist (0), this->myhface3(1), this->twist (1), f0, 1, this->subface3(3, 1), this->twist (3), this, 1, childVolume) ;
  assert(h0 && h1) ;
  h0->append(h1) ;
  _fc = f0 ;
  _dwn = h0 ;
  _rule = myrule_t :: e31 ;
  return ;
}

template < class A >  void TetraTop < A > :: 
splitISO8 () 
{
  typedef typename A :: myvertex_t  myvertex_t;
  typedef typename A :: inneredge_t inneredge_t;
  int l = 1 + this->level () ; 

  myvertex_t * e31 = this->myhface3 (0)->myhedge1 ((this->twist(0) < 0) ? ((9+this->twist(0))%3) : (this->twist(0)%3))->subvertex (0) ;
  myvertex_t * e20 = this->myhface3 (1)->myhedge1 ((this->twist(1) < 0) ? ((9+this->twist(1))%3) : (this->twist(1)%3))->subvertex (0) ;
  assert(e31 && e20);
  inneredge_t * e0 = new inneredge_t (l, e31, e20, getEdgeIndexManager() ) ;
  assert(e0) ;
  IndexManagerType & faceIdxMan = getFaceIndexManager();
  innerface_t * f0 = new innerface_t (l, this->subedge1 (3, 2), ((this->twist(3)>=0)?1:0), this->subedge1 (1, 2), ((this->twist(1)>=0)?1:0), this->subedge1 (2, 2), ((this->twist(2)>=0)?1:0), faceIdxMan ) ;
  innerface_t * f1 = new innerface_t (l, this->subedge1 (3, 0), ((this->twist(3)>=0)?1:0), this->subedge1 (2, 1), ((this->twist(2)>=0)?1:0), this->subedge1 (0, 2), ((this->twist(0)>=0)?1:0), faceIdxMan ) ;
  innerface_t * f2 = new innerface_t (l, this->subedge1 (3, 1), ((this->twist(3)>=0)?1:0), this->subedge1 (0, 1), ((this->twist(0)>=0)?1:0), this->subedge1 (1, 0), ((this->twist(1)>=0)?1:0), faceIdxMan ) ;
  innerface_t * f3 = new innerface_t (l, this->subedge1 (2, 0), ((this->twist(2)>=0)?0:1), this->subedge1 (0, 0), ((this->twist(0)>=0)?0:1), this->subedge1 (1, 1), ((this->twist(1)>=0)?0:1), faceIdxMan ) ;
  innerface_t * f4 = new innerface_t (l, e0, 0, this->subedge1 (3, 2), ((this->twist(3)>=0)?0:1), this->subedge1 (2, 1), ((this->twist(2)>=0)?1:0), faceIdxMan ) ;
  innerface_t * f5 = new innerface_t (l, e0, 0, this->subedge1 (3, 1), ((this->twist(3)>=0)?1:0), this->subedge1 (0, 2), ((this->twist(0)>=0)?0:1), faceIdxMan ) ; 
  innerface_t * f6 = new innerface_t (l, e0, 0, this->subedge1 (1, 0), ((this->twist(1)>=0)?0:1), this->subedge1 (0, 0), ((this->twist(0)>=0)?1:0), faceIdxMan ) ;
  innerface_t * f7 = new innerface_t (l, e0, 0, this->subedge1 (1, 2), ((this->twist(1)>=0)?1:0), this->subedge1 (2, 0), ((this->twist(2)>=0)?0:1), faceIdxMan ) ;
  assert(f0 && f1 && f2 && f3 && f4 && f5 && f6 && f7) ;
  f0->append(f1) ;
  f1->append(f2) ;
  f2->append(f3) ;
  f3->append(f4) ;
  f4->append(f5) ;
  f5->append(f6) ;
  f6->append(f7) ;

  // we divide by 8 means we divide the volume by 8
  const double childVolume = calculateChildVolume( 0.125 * _volume );
  
  // pointer `this' is the pointer to the father element 
  innertetra_t * h0 = new innertetra_t (l, f0, -1, this->subface3(1, 0), this->twist(1), this->subface3(2, 0), this->twist(2), this->subface3(3, 0), this->twist(3), this, 0 , childVolume) ;
  innertetra_t * h1 = new innertetra_t (l, this->subface3(0, 0), this->twist(0), f1, -3, this->subface3(2, 2), this->twist(2), this->subface3(3, 1), this->twist(3), this, 1 , childVolume) ;
  innertetra_t * h2 = new innertetra_t (l, this->subface3(0, 2), this->twist(0), this->subface3(1, 1), this->twist(1), f2, -1, this->subface3(3, 2), this->twist(3), this, 2 , childVolume) ;
  innertetra_t * h3 = new innertetra_t (l, this->subface3(0, 1), this->twist(0), this->subface3(1, 2), this->twist(1), this->subface3(2, 1), this->twist(2), f3, 0,  this, 3 , childVolume) ;
  innertetra_t * h4 = new innertetra_t (l, f7, -3, this->subface3(2, 3), ((this->twist(2)>=0) ? ((this->twist(2)+2)%3) : this->twist(2)) , f4, 2, f0, 0, this, 4 , childVolume) ;  
  innertetra_t * h5 = new innertetra_t (l, f4, -3, f1, 0, f5, 2, this->subface3(3, 3), ((this->twist(3)>=0) ? (this->twist(3)+1)%3 : (this->twist(3)-1)%3-1), this, 5 , childVolume) ;
  innertetra_t * h6 = new innertetra_t (l, f3, -1, f6, -3, this->subface3(1, 3), ((this->twist(1)>=0) ? this->twist(1) : this->twist(1)%3-1), f7, 1, this, 6 , childVolume) ;
  innertetra_t * h7 = new innertetra_t (l, this->subface3(0, 3), ((this->twist(0)>=0) ? (this->twist(0)+1)%3 : (this->twist(0)-1)%3-1), f5, -3, f2, 0, f6, 1, this, 7 , childVolume) ;
  assert(h0 && h1 && h2 && h3 && h4 && h5 && h6 && h7) ;
  h0->append(h1) ;
  h1->append(h2) ;
  h2->append(h3) ;
  h3->append(h4) ;
  h4->append(h5) ;
  h5->append(h6) ;
  h6->append(h7) ;
  _ed = e0 ;
  _fc = f0 ;
  _dwn = h0 ;
  _rule = myrule_t :: iso8 ;
  
  this->detachleafs();
  return ;
}

template < class A >  void TetraTop < A > :: refineImmediate (myrule_t r) 
{
  assert (getrule () == myrule_t :: nosplit) ;
  typedef typename myhface3_t :: myrule_t myhface3rule_t;

  switch(r) 
  {
    case myrule_t :: iso8 :
        
      // Das refineImmediate (..) auf allen Fl"achen wird vom tetra :: refine (..)
      // zwar nicht ben"otigt, da schliesslich alle Fl"achen sauber sind, wenn
      // "uberall hface3 :: refine (..) true geliefert hat, wohl aber z.B. von
      // restore () oder abgeleiteten Funktionen die eine direkte Verfeinerung
      // erzwingen m"ussen und d"urfen.
        
      {
        for (int i = 0 ; i < 4 ; i ++)
          this->myhface3 (i)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: iso4).rotate (this->twist (i))) ; 
      }
      splitISO8 () ;
      break ;
    case myrule_t :: e01 :
      this->myhface3 (2)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (2))) ;
      this->myhface3 (3)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (3))) ;
      split_e01 () ;
      break ;
    case myrule_t :: e12 :
      this->myhface3 (0)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (0))) ;
      this->myhface3 (3)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (3))) ;
      split_e12 () ;
      break ;
    case myrule_t :: e20 :
      this->myhface3 (1)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (1))) ;
      this->myhface3 (3)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (3))) ;
      split_e20 () ;
      break ;
    case myrule_t :: e23 :
      this->myhface3 (0)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (0))) ;
      this->myhface3 (1)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (1))) ;
      split_e23 () ;
      break ;
    case myrule_t :: e30 :
      this->myhface3 (1)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (1))) ;
      this->myhface3 (2)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (2))) ;
      split_e30 () ;
      break ;
    case myrule_t :: e31 :
      this->myhface3 (0)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (0))) ;
      this->myhface3 (2)->refineImmediate (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (2))) ;
      split_e31 () ;
      break ;
    default :
      cerr << "**FEHLER (FATAL) beim unbedingten Verfeinern mit unbekannter Regel: " ;
      cerr << "[" << r << "]. In " << __FILE__ << __LINE__ << endl ;
      abort () ;
      break ;
  }

  this->postRefinement () ;
  return ;
}

template < class A >  bool TetraTop < A > :: refine () 
{
  myrule_t r = _req ;
  if (r != myrule_t :: crs && r != myrule_t :: nosplit) 
  {
    if (r != getrule ()) 
    {
      assert (getrule () == myrule_t :: nosplit) ;
      _req = myrule_t :: nosplit ;
      switch (r) {
        typedef typename myhface3_t :: myrule_t  myhface3rule_t;
        case myrule_t :: crs :
        case myrule_t :: nosplit :
          return true ;
        case myrule_t :: e01 :
          if (!this->myhface3 (2)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (2)), this->twist (2))) return false ;
          if (!this->myhface3 (3)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (3)), this->twist (3))) return false ;
          break ;
  case myrule_t :: e12 :
          if (!this->myhface3 (0)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (0)), this->twist (0))) return false ;
          if (!this->myhface3 (3)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (3)), this->twist (3))) return false ;
          break ;
  case myrule_t :: e20 :
          if (!this->myhface3 (1)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (1)), this->twist (1))) return false ;
          if (!this->myhface3 (3)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (3)), this->twist (3))) return false ;
          break ;
  case myrule_t :: e23 :
          if (!this->myhface3 (0)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (0)), this->twist (0))) return false ;
          if (!this->myhface3 (1)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (1)), this->twist (1))) return false ;
          break ;
  case myrule_t :: e30 :
          if (!this->myhface3 (1)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (1)), this->twist (1))) return false ;
          if (!this->myhface3 (2)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (2)), this->twist (2))) return false ;
          break ;
  case myrule_t :: e31 :
          if (!this->myhface3 (0)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (0)), this->twist (0))) return false ;
          if (!this->myhface3 (2)->refine (myhface3rule_t (myhface3_t :: myrule_t :: e01).rotate (this->twist (2)), this->twist (2))) return false ;      
          break ;
  case myrule_t :: iso8 :
    {for (int i = 0 ; i < 4 ; ++i )
            if (!this->myhface3 (i)->refine (myhface3rule_t (myhface3_t :: myrule_t :: iso4).rotate (this->twist (i)), this->twist (i))) return false ; }
          break ;
        default :
          cerr << "**WARNUNG (FEHLER IGNORIERT) falsche Verfeinerungsregel [" << getrule () ;
          cerr << "] (ignoriert) in " << __FILE__ << " " << __LINE__ << endl ;
          return false ;
      }
      
  // Vorsicht: Im Fall eines konformen Verfeinerers mu"s hier die entstandene Verfeinerung
  // untersucht werden und dann erst das Element danach verfeinert werden.
      
      refineImmediate (r) ;
      return true ;
    }
  }
  return true ;
}

template < class A >  bool TetraTop < A > :: refineBalance (balrule_t r, int fce) {
  if (r != balrule_t :: iso4) {
    cerr << "**WARNUNG (IGNORIERT) in TetraTop < A > :: refineBalance (..) nachschauen, Datei " 
       << __FILE__ << " Zeile " << __LINE__ << endl ;
   
  // Bisher kann die Balancierung nur die isotrope Achtelung handhaben,
  // falls mehr gew"unscht wird muss es hier eingebaut werden. Im Moment wird
  // die Balancierung einfach verweigert, d.h. die Verfeinerung des anfordernden
  // Elements f"allt flach.
  
    return false ;
  }
  if (getrule () == myrule_t :: nosplit) {
    if (! this->myhface3 (fce)->leaf ()) {
      for (int i = 0 ; i < 4 ; i ++)
        if (i != fce)
          if (!this->myhface3 (i)->refine (balrule_t (balrule_t :: iso4).rotate (this->twist (i)), this->twist (i))) 
      return false ;
      _req = myrule_t :: nosplit ;
      refineImmediate (myrule_t :: iso8) ;
    }
  }
  return true ;
}

template < class A >  bool TetraTop < A > :: coarse () 
{
  if (this->leaf ()) 
  {
    assert (_req == myrule_t :: nosplit || _req == myrule_t :: crs) ;
    myrule_t w = _req ;
    _req = myrule_t :: nosplit ;
    // end recursion if rule is not croarsen 
    if (w != myrule_t :: crs) 
    {
      return false ;
    }
   
    // if I have faces that are not leaf, we cannot coarsen 
    for (int i = 0 ; i < 4 ; ++i) 
    {
      if ( ! this->myhface3 (i)->leaf ()) return false ;
    }
    // else coarsen 
    return true ;
  } 
  else 
  {
    assert (_req == myrule_t :: nosplit) ;
    bool x = true ;
    {
      for (innertetra_t * h = down () ; h ; h = h->next ()) x &= h->coarse () ; 
    }

    // if x is true, then all children are marked for coarsening and have
    // not faces that are not leaf 
    if (x) 
    {
      this->preCoarsening () ;
      this->attachleafs();
      delete _dwn ; 
      _dwn = 0 ;
      delete _fc ;
      _fc = 0 ;
      delete _ed ;
      _ed = 0 ;
      _rule = myrule_t :: nosplit ;
      {
        for (int i = 0 ; i < 4 ; ++i ) 
        {
          this->myneighbour (i).first->bndNotifyCoarsen () ;
          this->myhface3 (i)->coarse () ;
        }
      }
      return false ;
    }
  }
  return false ;
}

template < class A > void TetraTop < A > :: backupCMode (ostream & os) const {

  // Das backup im alten Stil, d.h. levelweise die Verfeinerungsregeln
  // vom Gitter runterschreiben. Diese Technik wird nur f"ur das backup
  // noch unterst"utzt, um die Daten mit "alteren Konstruktionen visual.
  // zu k"onnen.

  os << getrule () << " " ;
  return ;
}

// buckupTetra 
template < class A > void TetraTop < A > :: backupIndex (ostream & os) const 
{
#ifndef _DUNE_NOT_USES_ALU3DGRID_
  // write my index 
  os.write( ((const char *) & this->_idx ), sizeof(int) ) ;

  /*
  // write interior indices 
  {
    const vertex_STI * vx = this->innerVertex(); 
    if( vx ) vx->backupIndex( os );

    for(const hedge_STI * e = this->innerHedge () ; e ; e = e->next ())
    {
      e->backupIndex( os ); 
    }

    for(const hface_STI * f = this->innerHface () ; f ; f = f->next ())
    {
      f->backupIndex( os ); 
    }
  }
  */

  // write children 
  {
    for (const innertetra_t * c = down () ; c ; c = c->next ()) c->backupIndex (os) ; 
  }
#endif
  return;
}

// buckupTetra 
template < class A > void TetraTop < A > :: backup (ostream & os) const 
{
  doBackup( os );
}
template < class A > void TetraTop < A > :: backup (ObjectStream& os) const 
{
  doBackup( os );
}

template < class A > template <class OutStream_t> 
void TetraTop < A > :: doBackup (OutStream_t & os) const 
{
  os.put ((char) getrule ()) ;
  {for (const inneredge_t * e = innerHedge () ; e ; e = e->next ()) e->backup (os) ; }
  {for (const innerface_t * f = innerHface () ; f ; f = f->next ()) f->backup (os) ; }
  {for (const innertetra_t * c = down () ; c ; c = c->next ()) c->backup (os) ; }
  
  return ;
}

// overloaded restoreIndex Method 
template < class A > void TetraTop < A > :: 
restoreIndex (istream & is, vector<bool> (& isHole) [4] ) 
{
#ifndef _DUNE_NOT_USES_ALU3DGRID_
  // free index from constructor
  is.read ( ((char *) &(this->_idx) ), sizeof(int) );

  // mark this element a non hole 
  typedef typename Gitter :: Geometric :: BuilderIF BuilderIF; 

  // make sure sizes match 
  assert( this->getIndex() < (int) isHole[BuilderIF::IM_Elements].size() );
  // set entry to false, because this is not a hole  
  isHole[BuilderIF :: IM_Elements][this->getIndex()] = false;

  // TODO 
  // restore other indices 
  
  /*
  // write interior indices 
  {
    const vertex_STI * vx = this->innerVertex(); 
    if( vx ) vx->restoreIndex( is );

    for(const hedge_STI * e = this->innerHedge () ; e ; e = e->next ())
    {
      e->restoreIndex( is ); 
    }

    for(const hface_STI * f = this->innerHface () ; f ; f = f->next ())
    {
      f->restoreIndex( is ); 
    }
  }
  */

  {
    for (innertetra_t * c = down () ; c ; c = c->next ()) c->restoreIndex (is, isHole ) ; 
  }
#endif
  return;
}

// restoreTetra
template < class A > void TetraTop < A > :: restore (istream & is) 
{
  doRestore( is );
}

template < class A > void TetraTop < A > :: restore (ObjectStream & is) 
{
  doRestore( is );
}

template < class A > template <class InStream_t>
void TetraTop < A > :: doRestore (InStream_t & is) 
{
  // restore () stellt den Elementbaum aus der Verfeinerungs-
  // geschichte wieder her. Es ruft refine () auf und testet
  // auf den korrekten Vollzug der Verfeinerung. Danach werden
  // die inneren Gitterteile restore'd.
 
  myrule_t r ((char) is.get ()) ;
  assert(getrule() == myrule_t :: nosplit) ;
  if (r == myrule_t :: nosplit) 
  {
    // Vorsicht: beim restore m"ussen sich sowohl Element als auch
    // Randelement um die Korrektheit der Nachbarschaft k"ummern,
    // und zwar dann wenn sie "on the top" sind (= die gelesene
    // Verfeinerungsregel ist nosplit). (s.a. beim Randelement)
    // Die nachfolgende L"osung ist weit davon entfernt, sch"on
    // zu sein - leider. Eventuell wird mit der Verbesserung der
    // Behandlung der nichtkonf. Situationen mal eine "Anderung
    // n"otig.
  
    for (int i = 0 ; i < 4 ; ++i) 
    {
      myhface3_t & f (*(this->myhface3 (i))) ;
      if (!f.leaf ()) 
      {
        switch (f.getrule ()) 
        {
          case balrule_t :: e01 :
          case balrule_t :: e12 :
          case balrule_t :: e20 :
            { for (int j = 0 ; j < 2 ; j ++) f.subface3 (j)->nb.complete (f.nb) ;}
            break ;
          case balrule_t :: iso4 :
            { for (int j = 0 ; j < 4 ; j ++) f.subface3 (j)->nb.complete (f.nb) ; }
            break ;
          default :
            abort () ;
            break ;
        }
      }
    }
  } 
  else 
  {
    // Auf dem Element gibt es kein refine (myrule_t) deshalb mu"s erst
    // request (myrule_t) und dann refine () durchgef"uhrt werden.
  
    // request read rule 
    request (r) ;
    // refine tetra 
    refine() ;
    
    assert (getrule() == r) ;

    // call restore on inner items 
    { for (inneredge_t * e = innerHedge () ; e ; e = e->next ()) e->restore (is) ; }
    { for (innerface_t * f = innerHface () ; f ; f = f->next ()) f->restore (is) ; }
    
    // call restore on children 
    { 
      for (innertetra_t * c = down () ; c ; c = c->next ()) c->restore (is) ; 
    }
  }
  
  return ;
}

// ######                                                           #####  #######
// #     #  ######  #####      #     ####   #####      #     ####  #     #   #      ####   #####
// #     #  #       #    #     #    #    #  #    #     #    #    #       #   #     #    #  #    #
// ######   #####   #    #     #    #    #  #    #     #    #       #####    #     #    #  #    #
// #        #       #####      #    #    #  #    #     #    #            #   #     #    #  #####
// #        #       #   #      #    #    #  #    #     #    #    # #     #   #     #    #  #
// #        ######  #    #     #     ####   #####      #     ####   #####    #      ####   #
   
template < class A > typename Periodic3Top < A > :: myhedge1_t * Periodic3Top < A > :: subedge1 (int i, int j) {
  switch (this->myhface3(i)->getrule()) {
    case myhface3_t :: myrule_t :: e01 :
    case myhface3_t :: myrule_t :: e12 :
    case myhface3_t :: myrule_t :: e20 :
      assert( j == 0 );
      return this->myhface3 (i)->subedge1 (0) ;
    case myhface3_t :: myrule_t :: iso4 :
      assert( j < 3 );
      return ((this->twist (i) < 0) ? this->myhface3 (i)->subedge1 ((8 - j + this->twist (i)) % 3) : this->myhface3 (i)->subedge1 ((j + this->twist (i)) % 3)) ;
    case myhface3_t :: myrule_t :: nosplit :
      cerr << "**FEHLER (FATAL): subedge1 () auf nicht in verfeinerter Fl\"ache aufgerufen. In " << __FILE__ << " " << __LINE__ << endl ;
      abort () ;
      return 0 ;
  }
  return 0 ;
}

template < class A > const typename Periodic3Top < A > :: myhedge1_t * Periodic3Top < A > :: subedge1 (int i, int j) const {
  return ((Periodic3Top < A > *)this)->subedge1 (i,j) ;
}

template < class A > typename Periodic3Top < A > ::  myhface3_t * Periodic3Top < A > :: subface3 (int i, int j) {
  switch (this->myhface3 (i)->getrule ()) {
  case myhface3_t :: myrule_t :: e01 :
    assert( j < 2 );
    if ( this->twist(i) == 0 ||  this->twist(i) == 1  ||  this->twist(i) == -1 )
      return this->myhface3(i)->subface3(j) ;
    if ( this->twist(i) == 2 ||  this->twist(i) == -2 ||  this->twist(i) == -3 )
      return this->myhface3(i)->subface3(!j) ;
      cerr << __FILE__ << " " << __LINE__ << "myhface3(i)->subface3()" << endl;
      return 0;
  case myhface3_t :: myrule_t :: e12 :
    assert( j < 2 );
    if ( this->twist(i) == 0  ||  this->twist(i) == 2 ||  this->twist(i) == -3 )
      return this->myhface3(i)->subface3(j) ;
    if ( this->twist(i) == -1 ||  this->twist(i) == 1 ||  this->twist(i) == -2 )
      return this->myhface3(i)->subface3(!j) ;
    cerr << __FILE__ << " " << __LINE__ << "myhface3(i)->subface3()" << endl;
    return 0;
  case myhface3_t :: myrule_t :: e20 :
    assert( j < 2 );
    if ( this->twist(i) == 1 ||  this->twist(i) == 2 ||  this->twist(i) == -2 )
      return this->myhface3(i)->subface3(j) ;
    if ( this->twist(i) == 0 ||  this->twist(i) == -1 || this->twist(i) == -3 )
      return this->myhface3(i)->subface3(!j) ;
    cerr << __FILE__ << " " << __LINE__ << "myhface3(i)->subface3()" << endl;
    return 0;
  case myhface3_t :: myrule_t :: iso4 :
    assert( j < 4 );
    if ( j == 3 )
      return this->myhface3(i)->subface3 (3) ;
    if ( j < 3 )
      return this->myhface3 (i)->subface3 (this->twist(i) < 0 ? (7 - j + this->twist(i)) % 3 : (j + this->twist(i)) % 3) ;
  case myhface3_t :: myrule_t :: nosplit :
    cerr << "**FEHLER (FATAL): subface3 () auf nicht verfeinerter Fl\"ache aufgerufen. In " << __FILE__ << " " << __LINE__ << endl ;
    abort () ;
    return 0 ;
  default:
    cerr << "**FEHLER (FATAL): Falsche Verfeinerungsregel [" << this->myhface3(i)->getrule() << "] in "__FILE__ << " " << __LINE__ << endl ;
    abort() ;
  } 
  return 0 ;
}

template < class A > const typename Periodic3Top < A > ::  myhface3_t * Periodic3Top < A > :: subface3 (int i, int j) const {
  return ((Periodic3Top < A > *)this)->subface3 (i,j) ;
}

template < class A > void Periodic3Top < A > :: split_iso4 () 
{
  //von Hbnd kopiert ...
  this->splitGhosts();

  // get the childs 
  typedef typename Gitter :: ghostpair_STI ghostpair_STI;
  typedef typename Gitter :: Geometric :: tetra_GEO tetra_GEO;
  typedef typename Gitter :: Geometric :: hface3_GEO hface3_GEO;
  for (int g = 0; g < 2; ++g) 
  {
    ghostpair_STI ghostpair = this->getGhost(g);
    tetra_GEO * gh = static_cast<tetra_GEO *> (ghostpair.first); 

    // I hate this piece of code, R.K. 
    tetra_GEO *(ghchild)[4] = {0,0,0,0};
    int gFace[4] = {-1,-1,-1,-1};
    if(gh)
    {
      hface3_GEO * face = gh->myhface3( ghostpair.second ); 
      face = face->down();
      for( int i=0; i<4; i++)
      {
        assert(face);
        typedef pair < Gitter :: Geometric :: hasFace3 *, int > neigh_t;
        neigh_t neighbour = face->nb.front();
        tetra_GEO * ghch = static_cast<tetra_GEO *> (neighbour.first);
        if(ghch)
        { 
          if(ghch->up() != gh) 
          {
            neighbour = face->nb.rear();
            ghch = static_cast<tetra_GEO *> (neighbour.first);
          }
 
        }
        else 
        { 
          neighbour = face->nb.rear();
          ghch = static_cast<tetra_GEO *> (neighbour.first);
        }
      
        // gFace might be differnent from ghostFaceNumber, unfortuneately 
        gFace[i] = neighbour.second; 
       
        assert(ghch);
        assert(ghch->up() == gh);

        ghchild[i] = ghch;
        face = face->next();
      }
    }
  }
  //soweit von Hbnd
  
  int l = 1 + this->level () ;
  innerperiodic3_t * p0 = new innerperiodic3_t (l, this->subface3 (0,0), this->twist (0), this->subface3 (1,0), this->twist (1), this , 0) ;
  innerperiodic3_t * p1 = new innerperiodic3_t (l, this->subface3 (0,1), this->twist (0), this->subface3 (1,2), this->twist (1), this , 1) ;
  innerperiodic3_t * p2 = new innerperiodic3_t (l, this->subface3 (0,2), this->twist (0), this->subface3 (1,1), this->twist (1), this , 2) ;
  
  // Mir ist nicht ganz klar, warum der Twist auf diese seltsame Art umzurechnen ist,
  // die Zeile (bzw. die Formel) habe ich aus Mario's Tetradeder Split Iso-8 "uber-
  // nommen, ohne im einzelnen nachzupr"ufen, ob die Regel richtig ist. (BS)
  
  innerperiodic3_t * p3 = new innerperiodic3_t (l, this->subface3 (0,3), (this->twist(0) >= 0 ? (this->twist(0)+1)%3 : (this->twist(0)-1)%3-1), this->subface3 (1,3), (this->twist(1)>=0 ? (this->twist(1)+1)%3 : (this->twist(1)-1)%3-1) , this , 3) ;
  assert (p0 && p1 && p2 && p3) ;
  p0->append(p1) ;
  p1->append(p2) ;
  p2->append(p3) ;
  _dwn = p0 ;
  _rule = myrule_t :: iso4 ;
  p0->_up = p1->_up = p2->_up = p3->_up = this; //us
  return ;
}

template < class A > void Periodic3Top < A > :: refineImmediate (myrule_t r) {

  // Die Methode wird nur vom restore () und vom refineBalance () auf-
  // gerufen und geht davon aus, dass das betroffene Element noch nicht
  // verfeinert ist -> ist ein Blatt der Hierarchie.

  assert (this->leaf()) ;
  switch(r) {
    case myrule_t :: e01 :
    case myrule_t :: e12 :
    case myrule_t :: e20 :
    
      // Mit den drei anisotropen Regeln k"onnen wir leider noch nichts anfangen.
    
      abort () ;
    case myrule_t :: iso4 :
    
  // Das refineImmediate (..) auf allen Fl"achen wird vom periodic3 :: refine (..)
  // zwar nicht ben"otigt, da schliesslich alle Fl"achen sauber sind, wenn
  // "uberall hface3 :: refine (..) true geliefert hat, wohl aber z.B. von
  // restore () oder abgeleiteten Funktionen die eine direkte Verfeinerung
  // erzwingen m"ussen und d"urfen.
   
      typedef typename myhface3_t :: myrule_t myhface3rule_t;
      this->myhface3 (0)->refineImmediate (myhface3rule_t (r).rotate (this->twist (0))) ;
      this->myhface3 (1)->refineImmediate (myhface3rule_t (r).rotate (this->twist (1))) ;
      split_iso4 () ;
      break ;
    default :
      cerr << "**FEHLER (FATAL) beim unbedingten Verfeinern mit unbekannter Regel: " ;
      cerr << "[" << r << "]. In " << __FILE__ << __LINE__ << endl ;
      abort () ;
      break ;
  }
  this->postRefinement () ;
  return ;
}

template < class A > bool Periodic3Top < A > :: refine () {

  // Das refine () reagiert nicht auf die Elementaktivierung zur Verfeinerung
  // in der globalen Schleife, weil das perioodische Randelement sich nur auf
  // Anforderung zur Balancierung aus einem anliegenden Element direkt verfeinert.

  return true ;
}

template < class A > bool Periodic3Top < A > :: refineBalance (balrule_t r, int fce) {
  if (r != balrule_t :: iso4) {
    cerr << "**WARNUNG (IGNORIERT) in Periodic3Top < A > :: refineBalance (..) nachschauen, Datei " 
       << __FILE__ << " Zeile " << __LINE__ << endl ;
   
  // Bisher kann die Balancierung nur die isotrope Achtelung handhaben,
  // falls mehr gew"unscht wird, muss es hier eingebaut werden. Im Moment wird
  // die Balancierung einfach verweigert, d.h. die Verfeinerung des anfordernden
  // Elements f"allt flach.
  
    return false ;
  } else {
  
  // Der nachfolgende Aufruf nutzt aus, dass die Regel der periodischen R"ander
  // sich direkt auf die Balancierungsregel des entsprechenden Polygonverbinders
  // projezieren l"asst (n"amlich 1:1). Deshalb unterscheidet der Aufruf nicht nach
  // der angeforderten Regel in einer 'case' Anweisung.
  
    int opp = fce == 0 ? 1 : 0 ;
    if (this->myhface3 (opp)->refine (typename myhface3_t :: myrule_t (r).rotate (this->twist (opp)), this->twist (opp))) {
      refineImmediate (r) ;
      return true ;
    } else {
      return false ;
    }
  }
}

template < class A > bool Periodic3Top < A > :: coarse () {

  // Das Vergr"obern geschieht auch passiv, sobald ein anliegendes Element
  // vergr"obert wird durch den Aufruf von "bndNotifyCoarsen ()" s.u.

  bndNotifyCoarsen () ;
  return false ;
}

template < class A > bool Periodic3Top < A > :: bndNotifyCoarsen () {

  // Wie beim Randelement auch: Die Vergr"oberung eines anliegenden Elements
  // l"ost einen Vorgang aus, der feststellt ob das periodische RE ebenfalls
  // vergr"obert werden soll.
  
  innerperiodic3_t * p = down () ;
  if (!p) return false ;
  bool x = true ;
  do {

  // Falls p kein Blatt der Hierarchie ist, 
  // die Vergr"oberungsm"oglichkeit weitergeben.

    if (!p->leaf ()) p->coarse () ;

  // F"ur die hintere und vordere Fl"ache feststellen, ob
  // der Referenzenz"ahler mehr als einen Eintrag ergibt.

    if (p->myhface3 (0)->ref > 1) (x = false) ;
    if (p->myhface3 (1)->ref > 1) (x = false) ;

  } while ( (p = p->next ()) ) ;
  if (x) {
  
  // Falls keine Fl"achen anliegen, die auf Kinder oder Kindes- 
  // mit mehr als einer Referenz f"uhren, ist sicher, dass das
  // Bezugsrandelement zwischen zwei 'relativ groben' Elementen
  // liegt. Somit kann es vergr"obert werden.
  
    this->preCoarsening () ;
    
    //mit Geistern (us)
    this->coarseGhosts();
    
    delete _dwn ;
    _dwn = 0 ;
    _rule = myrule_t :: nosplit ;
    this->myhface3 (0)->coarse () ;
    this->myhface3 (1)->coarse () ;
  }
  return x ;
}

template < class A > void Periodic3Top < A > :: backupCMode (ostream & os) const {

  // Das backup im alten Stil, d.h. levelweise die Verfeinerungsregeln
  // vom Gitter runterschreiben. Diese Technik wird nur f"ur das backup
  // noch unterst"utzt, um die Daten mit "alteren Konstruktionen visual.
  // zu k"onnen.
  
  os << getrule () << " " ;
  return ;
}

template < class A > void Periodic3Top < A > :: backup (ostream & os) const 
{
  doBackup( os );
}
template < class A > void Periodic3Top < A > :: backup (ObjectStream& os) const 
{
  doBackup( os );
}

template < class A > template <class OutStream_t>
void Periodic3Top < A > :: doBackup (OutStream_t& os) const 
{
  os.put ((char) getrule ()) ;
  { for (const innerperiodic3_t * c = down () ; c ; c = c->next ()) c->backup (os) ; }
  return ;
}

template < class A > void Periodic3Top < A > :: restore (istream & is) 
{
  doRestore( is );
}
template < class A > void Periodic3Top < A > :: restore (ObjectStream& is) 
{
  doRestore( is );
}

template < class A > template <class InStream_t> 
void Periodic3Top < A > :: doRestore (InStream_t & is) 
{
  myrule_t r ((char) is.get ()) ;
  assert(getrule () == myrule_t :: nosplit) ; // Testen auf unverfeinerten Zustand
  if (r == myrule_t :: nosplit) {
    for (int i = 0 ; i < 2 ; i ++) {
      myhface3_t & f (*(this->myhface3 (i))) ;
      if (!f.leaf ()) {
        switch (f.getrule ()) {
    case balrule_t :: iso4 :
            {for (int j = 0 ; j < 4 ; j ++) f.subface3 (j)->nb.complete (f.nb) ;}
      break ;
    default :
      cerr << "**FEHLER (FATAL) beim restore mit unbekannter Balancierungsregel: "
                 << "[" << r << "]. In " << __FILE__ << __LINE__ << endl ;
      abort () ;
      break ;
  }
      }
    }
  } 
  else {
    refineImmediate (r) ;
    assert (getrule() == r) ;
    {for (innerperiodic3_t * c = down () ; c ; c = c->next ()) c->restore (is) ; }
  }
  return ;
}


// template instantiation 
#ifndef GITTER_TETRA_TOP_PLL_H_INCLUDED
template class Hface3Top< GitterBasis :: Objects :: Hface3Empty > ;
template class Hbnd3Top< GitterBasis :: Objects :: Hbnd3Default > ;
template class TetraTop < GitterBasis :: Objects :: TetraEmpty > ;
template class Periodic3Top < GitterBasis :: Objects :: Periodic3Empty > ;
#endif