// (c) Robert Kloefkorn 2010

#include "mapp_cube_3d.h"
#include "gitter_hexa_top.h"


// #     #                                    #    #######
// #     #  ######  #####    ####   ######   ##       #      ####   #####
// #     #  #       #    #  #    #  #       # #       #     #    #  #    #
// #######  #####   #    #  #       #####     #       #     #    #  #    #
// #     #  #       #    #  #  ###  #         #       #     #    #  #####
// #     #  #       #    #  #    #  #         #       #     #    #  #
// #     #  ######  #####    ####   ######  #####     #      ####   #


template < class A > void Hedge1Top < A > :: refineImmediate (myrule_t r) 
{
  if (r != getrule ()) {
    assert (getrule () == myrule_t :: nosplit) ;
    switch (r) {
      case myrule_t :: iso2 :
        {
          int l = 1 + level () ;
          assert ( _inner == 0 ) ;
         
          innervertex_t* v0 = static_cast<innervertex_t *> (this->myvertex(0));
          innervertex_t* v1 = static_cast<innervertex_t *> (this->myvertex(1));
          // get the vertex coordinates   
          const alucoord_t (&p0)[3] = v0->Point();
          const alucoord_t (&p1)[3] = v1->Point();
          
          // the last myvertex(0) is submitted for the indexmanager reference, rk
          _inner = new inner_t (l, 
                                0.5 * (p0[0] + p1[0]),
                                0.5 * (p0[1] + p1[1]),  
                                0.5 * (p0[2] + p1[2]), 
                                *v0 ) ;
          assert (_inner) ;

#ifdef USE_MALLOC_AT_ONCE
          void* edgeMem[2] = {0,0};
          this->mallocAtOnce( sizeof(inneredge_t), edgeMem, 2 );

          inneredge_t * e0 = new (edgeMem[0]) inneredge_t (l, v0 , inVx(), 0 ) ;
          inneredge_t * e1 = new (edgeMem[1]) inneredge_t (l, inVx(), v1, 1 ) ;
#else
          inneredge_t * e0 = new inneredge_t (l, v0 , inVx(), 0 ) ;
          inneredge_t * e1 = new inneredge_t (l, inVx(), v1, 1 ) ;
#endif

          assert (e0 && e1) ;
          e0->append (e1) ;
          _inner->store( e0 );
          _rule = myrule_t :: iso2 ;
          break ;
        }
      default :
        cerr << "**FEHLER (FATAL) falsche Verfeinerungsregel [" << r ;
        cerr << "] in " << __FILE__ << " " << __LINE__ << endl ;
        abort () ;
        break ;
    }
    this->postRefinement () ;
  }
  return ;
}

template < class A > bool Hedge1Top < A > :: coarse () {
  if (this->leaf ()) return false ;
  bool x = true ;

  // Der Wert von x bleibt 'true' falls alle Kinder der Kante
  // Bl"atter sind und zudem keine Referenzen auf diese Kanten
  // gesetzt sind. Andernfalls liegt kein vergr"oberungsf"ahiger
  // Knoten vor.
  // Vorsicht: Im parallelen Gitter bleiben auch Kanten ohne
  // Refcount stehen, um konsistente "Uberg"ange zu erhalten.

  for (inneredge_t * f = dwnPtr() ; f ; f = f->next ()) {
    if (f->leaf ()) {
      x &= ! f->ref ;
    } else {
      x = false ;
      f->coarse () ;
    }
  }
  if (x) {

  // Falls lockedAgainstCoarsening () aufgerufen 'true' liefert
  // soll die Operation des Vergr"oberns nicht sofort ausgef"uhrt
  // sondern (pending) zur"uckgestellt werden.

    if (!this->lockedAgainstCoarsening ()) 
    {
      delete _inner ;  
      _inner = 0 ;
      _rule = myrule_t :: nosplit ;
    }
  }
  return x ;
}

// #     #                                 #       #######
// #     #  ######    ##     ####   ###### #    #     #      ####   #####
// #     #  #        #  #   #    #  #      #    #     #     #    #  #    #
// #######  #####   #    #  #       #####  #    #     #     #    #  #    #
// #     #  #       ######  #       #      #######    #     #    #  #####
// #     #  #       #    #  #    #  #           #     #     #    #  #
// #     #  #       #    #   ####   ######      #     #      ####   #


template < class A >  void Hface4Top < A > :: splitISO4 () {
  int l = 1 + level () ;
  assert ( _inner == 0 ) ;

  {
    // calculate barycenter of face 
    innervertex_t* v0 = static_cast<innervertex_t *> (this->myvertex (0));
    alucoord_t p [3] ;
    BilinearSurfaceMapping :: barycenter(
          v0->Point(),
          this->myvertex (1)->Point(), 
          this->myvertex (2)->Point(), 
          this->myvertex (3)->Point(),
          p ) ;

    // myvertex(0) is submitted for the indexmanager reference 
    _inner = new inner_t (l, p[0], p[1], p[2], *v0 ) ;
    assert (_inner) ;
  }
  
  myvertex_t * ev0 = this->myhedge1(0)->subvertex (0) ;
  myvertex_t * ev1 = this->myhedge1(1)->subvertex (0) ;
  myvertex_t * ev2 = this->myhedge1(2)->subvertex (0) ;
  myvertex_t * ev3 = this->myhedge1(3)->subvertex (0) ;
  assert(ev0 && ev1 && ev2 && ev3) ;

#ifdef USE_MALLOC_AT_ONCE
  void* edgeMem[4] = {0,0,0,0};
  this->mallocAtOnce( sizeof(inneredge_t), edgeMem, 4 );

  inneredge_t * e0 = new (edgeMem[0]) inneredge_t (l, ev0, inVx()) ;
  inneredge_t * e1 = new (edgeMem[1]) inneredge_t (l, ev1, inVx()) ;
  inneredge_t * e2 = new (edgeMem[2]) inneredge_t (l, ev2, inVx()) ;
  inneredge_t * e3 = new (edgeMem[3]) inneredge_t (l, ev3, inVx()) ;
#else
  inneredge_t * e0 = new inneredge_t (l, ev0, inVx()) ;
  inneredge_t * e1 = new inneredge_t (l, ev1, inVx()) ;
  inneredge_t * e2 = new inneredge_t (l, ev2, inVx()) ;
  inneredge_t * e3 = new inneredge_t (l, ev3, inVx()) ;
#endif
  assert( e0 && e1 && e2 && e3) ;
  e0->append(e1) ;
  e1->append(e2) ;
  e2->append(e3) ;

#ifdef USE_MALLOC_AT_ONCE
  void* faceMem[4] = {0,0,0,0};
  this->mallocAtOnce( sizeof(innerface_t), faceMem, 4 );

  innerface_t * f0 = new (faceMem[0]) innerface_t (l, this->subedge1(0,0), this->twist(0), e0, 0, e3, 1, this->subedge1(3,1), this->twist(3), 0) ;
  innerface_t * f1 = new (faceMem[1]) innerface_t (l, this->subedge1(0,1), this->twist(0), this->subedge1(1,0), this->twist(1), e1, 0, e0, 1, 1) ;
  innerface_t * f2 = new (faceMem[2]) innerface_t (l, e1, 1, this->subedge1(1,1), this->twist(1), this->subedge1(2,0), this->twist(2), e2, 0, 2) ;
  innerface_t * f3 = new (faceMem[3]) innerface_t (l, e3, 0, e2, 1, this->subedge1(2,1), this->twist(2), this->subedge1(3,0), this->twist(3), 3) ;
#else 
  innerface_t * f0 = new innerface_t (l, this->subedge1(0,0), this->twist(0), e0, 0, e3, 1, this->subedge1(3,1), this->twist(3), 0) ;
  innerface_t * f1 = new innerface_t (l, this->subedge1(0,1), this->twist(0), this->subedge1(1,0), this->twist(1), e1, 0, e0, 1, 1) ;
  innerface_t * f2 = new innerface_t (l, e1, 1, this->subedge1(1,1), this->twist(1), this->subedge1(2,0), this->twist(2), e2, 0, 2) ;
  innerface_t * f3 = new innerface_t (l, e3, 0, e2, 1, this->subedge1(2,1), this->twist(2), this->subedge1(3,0), this->twist(3), 3) ;
#endif

  assert (f0 && f1 && f2 && f3) ;  
  f0->append(f1) ;
  f1->append(f2) ;
  f2->append(f3) ;
  // inner edge 
  _inner->store( e0 );
  // down pointer 
  _inner->store( f0 );
  _rule = myrule_t :: iso4 ;
  return ;
}

template < class A > void Hface4Top < A > :: refineImmediate (myrule_t r) {
  if (r != getrule ()) {
    assert (getrule () == myrule_t :: nosplit) ;
    switch(r) {
      typedef typename myhedge1_t :: myrule_t myhedge1rule_t;
      case myrule_t :: iso4 :
      this->myhedge1 (0)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (0))) ;
      this->myhedge1 (1)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (1))) ;
      this->myhedge1 (2)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (2))) ;
      this->myhedge1 (3)->refineImmediate (myhedge1rule_t (myhedge1_t :: myrule_t :: iso2).rotate (this->twist (3))) ;
      splitISO4 () ;
      break ;
    default :
      cerr << "**FEHLER (FATAL) falsche Verfeinerungsregel [" << r ;
      cerr << "] in " << __FILE__ << " " << __LINE__ << endl ;
      abort () ;
      break ;
    }

    // * higher order, this is a hack
    for (innerface_t* f = down(); f; f = f->next()) 
    { 
      f->nb._parRule = getrule();
    }

    this->postRefinement () ;
  }
  return ;
}

template < class A > bool Hface4Top < A > :: refine (myrule_t r, int twist) {
  if (r != getrule ()) {
    assert (getrule () == myrule_t :: nosplit ? 1 : 
      (cerr << "**FEHLER beim Verfeinern mit Regel " << r << " auf " << getrule () << endl, 0)) ;
    switch(r) {
      case myrule_t :: iso4 :
      {

  bool a = (twist < 0) 
          ? this->nb.front ().first->refineBalance (r,this->nb.front ().second)
          : this->nb.rear  ().first->refineBalance (r,this->nb.rear  ().second) ;
  
  if (a) {  
    if (getrule () == myrule_t :: nosplit) {
      refineImmediate (r) ;
      {for (innerface_t * f = dwnPtr() ; f ; f = f->next ()) f->nb = this->nb ; }
    } else {
      assert (getrule () == myrule_t :: iso4) ;
    }
    return true ;
  } else {
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

template < class A > bool Hface4Top < A > :: coarse () {
  innerface_t * f = down() ;
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
    
    delete _inner; 
    _inner = 0;

    _rule = myrule_t :: nosplit ;
    {for (int i = 0 ; i < 4 ; i ++ ) this->myhedge1 (i)->coarse () ; }
  }
  return x ;
}

// #     #                         #       #######
// #     #  #####   #    #  #####  #    #     #      ####   #####
// #     #  #    #  ##   #  #    # #    #     #     #    #  #    #
// #######  #####   # #  #  #    # #    #     #     #    #  #    #
// #     #  #    #  #  # #  #    # #######    #     #    #  #####
// #     #  #    #  #   ##  #    #      #     #     #    #  #
// #     #  #####   #    #  #####       #     #      ####   #


template < class A >  void Hbnd4Top < A > :: 
setBoundaryId( const int id ) 
{
  myhface4_t & face = *(this->myhface4(0));
  face.setBndId( id );
  // 4 fertices and edges  
  for(int i=0; i<4; ++i)
  {
    face.myvertex(i)->setBndId( id );
    face.myhedge1(i)->setBndId( id );
  }
}

template < class A > bool Hbnd4Top < A > :: coarse () {
  innerbndseg_t * b = down () ;
  if (!b) return false ;
  bool x = true ;
  do {
    if(b->myhface4(0)->ref > 1) (b->coarse (), x = false) ;
  } while ( (b = b->next()) ) ;
  if (x) {
    if (! this->lockedAgainstCoarsening ()) {

      this->preCoarsening () ;
      this->coarseGhost();
      
      delete _dwn ; _dwn = 0 ;
      this->myhface4 (0)->coarse () ;
    }
  }
  return x ;
}

template < class A >  bool Hbnd4Top < A > :: bndNotifyCoarsen () {
  return coarse () ;
}

template < class A >  void Hbnd4Top < A > :: splitISO4 () {
  int l = 1 + level () ;
  assert (_dwn == 0) ;

  // refine ghost element and fill ghost info 
  typedef typename Gitter :: GhostChildrenInfo GhostChildrenInfo; 
  GhostChildrenInfo ghostInfo;
  // ghostInfo is filled by splitGhost, see gitter_hexa_top_pll.h  
  this->splitGhost( ghostInfo );

#ifdef USE_MALLOC_AT_ONCE
  void* bndMem[8] = {0,0,0,0};
  this->mallocAtOnce( sizeof(innerbndseg_t), bndMem, 4 );

  innerbndseg_t * b0 = new (bndMem[0]) innerbndseg_t (l, this->subface4 (0,0), this->twist (0), this, ghostInfo.child(0), ghostInfo.face(0)) ;
  innerbndseg_t * b1 = new (bndMem[1]) innerbndseg_t (l, this->subface4 (0,1), this->twist (0), this, ghostInfo.child(1), ghostInfo.face(1)) ;
  innerbndseg_t * b2 = new (bndMem[2]) innerbndseg_t (l, this->subface4 (0,2), this->twist (0), this, ghostInfo.child(2), ghostInfo.face(2)) ;
  innerbndseg_t * b3 = new (bndMem[3]) innerbndseg_t (l, this->subface4 (0,3), this->twist (0), this, ghostInfo.child(3), ghostInfo.face(3)) ;
#else
  innerbndseg_t * b0 = new innerbndseg_t (l, this->subface4 (0,0), this->twist (0), this, ghostInfo.child(0), ghostInfo.face(0)) ;
  innerbndseg_t * b1 = new innerbndseg_t (l, this->subface4 (0,1), this->twist (0), this, ghostInfo.child(1), ghostInfo.face(1)) ;
  innerbndseg_t * b2 = new innerbndseg_t (l, this->subface4 (0,2), this->twist (0), this, ghostInfo.child(2), ghostInfo.face(2)) ;
  innerbndseg_t * b3 = new innerbndseg_t (l, this->subface4 (0,3), this->twist (0), this, ghostInfo.child(3), ghostInfo.face(3)) ;
#endif 
  assert (b0 && b1 && b2 && b3) ;
  b0->append(b1) ;
  b1->append(b2) ;
  b2->append(b3) ;
  _dwn = b0 ;
  return ;
}

template < class A >  bool Hbnd4Top < A > :: refineBalance (balrule_t r, int b) 
{
  // Die Methode refineBalance () f"uhrt auf dem Randabschluss entweder
  // unbedingt die Verfeinerung durch, da im Verlauf der Verfeinerung keine
  // weiteren Anforerungen mehr an den Randabschluss  gerichtet werden 
  // ODER gibt die Verfeinerung als nicht erf"ullt zur"uck: Dann liegt
  // es am Aufrufer die Verfeinerung nochmals anzuforern.

  assert (b == 0) ;
  assert (this->leaf ()) ;
  if (! bndNotifyBalance (r,b)) 
  {
    // Hier kann der innere Rand [parallel] die Verfeinerung
    // verhindern, damit z.B. das Durchverfeinern im anisotropen
    // Fall erstmal nicht stattfindet, wenn nicht klar ist, wie die
    // weitere Rekursion aussieht. Dazu muss auf dem Niveau der Klasse
    // des Template-Arguments die Methode bndNotifyBalance () "uber-
    // schrieben werden. Die Defaultmethode liefert immer 'true'.
    
    return false ;
  } 
  else 
  {
    if(r == myrule_t :: iso4) 
    {
      // Der Rand verfeinert unbedingt die anliegende Fl"ache und dann
      // sich selbst, weil die Anforderung durch die Fl"ache kam, und
      // dahinter keine Balancierung stattfinden muss.
    
      // refine face 
      this->myhface4 (0)->refineImmediate (r) ;
      // refine myself 
      splitISO4 () ;
    } 
    else 
    {
      cerr << "**FEHLER (FATAL, weil nicht vorgesehen) beim Verfeinern am " ;
      cerr << "Randst\"uck mit der Regel [" << r << "] in " ;
      cerr << __FILE__ << " " << __LINE__ << endl ;
      abort () ;
    }
    
    // postRefinement () gibt die M"oglichkeit auf dem Niveau des
    // Template-Arguments eine Methode aufzurufen, um eventuelle
    // Operationen auf dem verfeinerten Randst"uck aufzurufen.
    
    this->postRefinement () ;
    return true ;
  }
}

template < class A >  bool Hbnd4Top < A > :: refineLikeElement (balrule_t r) 
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

  if (r == myrule_t :: nosplit) 
  {
    cerr << "**WARNUNG (IGNORIERT) beim Versuch mit nosplit zu Verfeinern" ;
    cerr << "  in " << __FILE__ << " " << __LINE__ << endl ;
    
    // Eine Anforderung mit nosplit zu Verfeinern nur erf"ullt,
    // falls die zugeh"orige Fl"achenregel auch nosplit ist, sonst
    // wird die Anforderung als nicht erf"ullt zur"uckgegeben.
    
    return this->getrule () == balrule_t :: nosplit ? true : false ;
  } 
  else 
  {
    if (this->getrule () == r) 
    {
      // Alles schon wie es sein soll -> true.
      return true ;
    } 
    else 
    {
      // Der nachfolgende Test bezieht sich auf die Verfeinerungssituation
      // der Fl"ache, da getrule () auf myhface4 (0)->getrule () umgeleitet
      // ist.
      
      assert (this->getrule () == myrule_t :: nosplit) ;
      switch (r) 
      {
      case balrule_t :: iso4 :
        if (! this->myhface4 (0)->refine(balrule_t (balrule_t :: iso4).rotate (this->twist (0)), this->twist (0))) return false ;

        // call refinement method 
        splitISO4 () ;

        // postRefinement () gibt die M"oglichkeit auf dem Niveau des
        // Template-Arguments eine Methode aufzurufen, um eventuelle
        // Operationen auf dem verfeinerten Randst"uck aufzurufen.
        this->postRefinement () ;

        return true ;
      default :
        cerr << "**WARNUNG (FEHLER IGNORIERT) falsche Verfeinerungsregel [" << this->getrule () ;
        cerr << "] (ignoriert) in " << __FILE__ << " " << __LINE__ << endl ;
        return false ;
      }
    }
  }
}

template < class A > void Hbnd4Top < A > :: restoreFollowFace () 
{
  // retoreFollowFace () veranlasst das Randelement sich am 
  // bestehenden Fl"achenbaum wiederherzustellen durch die
  // entsprechende Verfeinerung.
  
  myhface4_t & f (*(this->myhface4 (0))) ;
  if (!f.leaf ()) 
  {
    balrule_t r = f.getrule () ;
    switch (r) 
    {
      case myrule_t :: iso4 :
        splitISO4 () ;
        break ;
      default :
        cerr << "**FEHLER (FATAL, weil nicht vorgesehen) beim Verfeinern am " ;
        cerr << "Randst\"uck mit der Regel [" << r << "] in " ;
        cerr << __FILE__ << " " << __LINE__ << endl ;
        abort () ;
        break ;
    }
    // do post refinement 
    this->postRefinement () ;
    {
      for (innerbndseg_t * b = down () ; b ; b = b->next ()) b->restoreFollowFace () ; 
    }
  }
  return ;
}

// #     #                         #######
// #     #  ######  #    #    ##      #      ####   #####
// #     #  #        #  #    #  #     #     #    #  #    #
// #######  #####     ##    #    #    #     #    #  #    #
// #     #  #         ##    ######    #     #    #  #####
// #     #  #        #  #   #    #    #     #    #  #
// #     #  ######  #    #  #    #    #      ####   #

// constructor for macro elements  
template < class A > HexaTop < A > 
:: HexaTop (int l, myhface4_t * f0, int t0, myhface4_t * f1, int t1, 
            myhface4_t * f2, int t2, myhface4_t * f3, int t3, myhface4_t * f4, 
            int t4, myhface4_t * f5, int t5) 
  : A (f0, t0, f1, t1, f2, t2, f3, t3, f4, t4, f5, t5 )
  , _bbb (0), _up(0)
  , _inner( 0 )
  , _volume (0.0) 
  , _lvl (l)
  , _nChild(0) 
  , _rule (myrule_t :: nosplit), _req (myrule_t :: nosplit) 
  , _affine( false )
{ 
  TrilinearMapping trMap (this->myvertex(0)->Point(), this->myvertex(1)->Point(),
                          this->myvertex(2)->Point(), this->myvertex(3)->Point(),
                          this->myvertex(4)->Point(), this->myvertex(5)->Point(),
                          this->myvertex(6)->Point(), this->myvertex(7)->Point());
  // calculate volume 
  _volume = QuadraturCube3D < VolumeCalc > (trMap).integrate2 (0.0);
  // check whether mapping is affine 
  _affine = trMap.affine(); 

  assert( this->level() == l );
  
  this->setIndex( indexManager().getIndex() );   
  return ;
}

// constructor for refinement 
template < class A > HexaTop < A > 
:: HexaTop (int l, myhface4_t * f0, int t0, myhface4_t * f1, int t1, 
            myhface4_t * f2, int t2, myhface4_t * f3, int t3, myhface4_t * f4, 
            int t4, myhface4_t * f5, int t5, innerhexa_t * up , int nChild , double vol ) 
  : A (f0, t0, f1, t1, f2, t2, f3, t3, f4, t4, f5, t5 )
  , _bbb (0), _up(up)
  , _inner( 0 )
  , _volume ( vol )
  , _lvl (l)
  , _nChild(nChild) 
  , _rule (myrule_t :: nosplit), _req (myrule_t :: nosplit)
  , _affine(_up->_affine)
{ 
  assert( this->level() == l );

  this->setIndex( indexManager().getIndex() );   

  // set bndid to fathers bndid now 
  this->_bndid = _up->bndId();

  // if mapping is not affine recalculate volume 
  if( ! _affine )
  {
    TrilinearMapping triMap (this->myvertex(0)->Point(),
                             this->myvertex(1)->Point(),
                             this->myvertex(2)->Point(),
                             this->myvertex(3)->Point(),
                             this->myvertex(4)->Point(),
                             this->myvertex(5)->Point(),
                             this->myvertex(6)->Point(),
                             this->myvertex(7)->Point() );

#ifndef NDEBUG 
    // make sure determinant is ok 
    alucoord_t point[3] = { 0.0, 0.0, 0.0 };
    assert( triMap.det( point ) > 0 );
#endif

    // calculate volume 
    _volume = QuadraturCube3D < VolumeCalc > (triMap).integrate2 (0.0);
  }

  // make sure that given volume is the same as calulated 
  assert( fabs (
      QuadraturCube3D < VolumeCalc >
       (TrilinearMapping (this->myvertex(0)->Point(), this->myvertex(1)->Point(),
                          this->myvertex(2)->Point(), this->myvertex(3)->Point(),
                          this->myvertex(4)->Point(), this->myvertex(5)->Point(),
                          this->myvertex(6)->Point(), this->myvertex(7)->Point())).integrate2 (0.0) - _volume) < 1e-10 );
  return ;
}

template < class A > HexaTop < A > :: ~HexaTop () 
{
  this->freeIndex( indexManager() );
    
  if (! _inner ) this->detachleafs();
  else assert(!this->isLeafEntity());
  if (_bbb) delete _bbb ;
  if (_inner) delete _inner;
  return ;
}

template < class A >  typename HexaTop < A > :: myhedge1_t * HexaTop < A > :: subedge1 (int i, int j) {
  return (j < 4) ? ((this->twist (i) < 0) ? this->myhface4 (i)->myhedge1 ((8 - j + this->twist (i)) % 4) : 
    this->myhface4 (i)->myhedge1 ((j + this->twist (i)) % 4)) : 
    ((this->twist (i) < 0) ? this->myhface4 (i)->subedge1 ((12 - j + this->twist (i)) % 4) : 
    this->myhface4 (i)->subedge1 ((j + this->twist (i)) % 4)) ;
}
    
template < class A >  const typename HexaTop < A > :: myhedge1_t * HexaTop < A > :: subedge1 (int i, int j) const {
  return (j < 4) ? ((this->twist (i) < 0) ? this->myhface4 (i)->myhedge1 ((8 - j + this->twist (i)) % 4) : 
      this->myhface4 (i)->myhedge1 ((j + this->twist (i)) % 4)) : 
  ((this->twist (i) < 0) ? this->myhface4 (i)->subedge1 ((12 - j + this->twist (i)) % 4) : 
  this->myhface4 (i)->subedge1 ((j + this->twist (i)) % 4)) ;
}
    
template < class A >  typename HexaTop < A > :: myhface4_t * HexaTop < A > :: subface4 (int i, int j) {
  return (this->myhface4(i)->getrule() == myhface4_t :: myrule_t :: iso4) ? 
  this->myhface4(i)->subface4(this->twist(i) < 0 ? (9 - j + this->twist(i)) % 4 : (j + this->twist(i)) % 4) :
  (abort (), (myhface4_t *)0) ;
}
    
template < class A >  const typename HexaTop < A > :: myhface4_t * HexaTop < A > :: subface4 (int i, int j) const {
  return (this->myhface4(i)->getrule() == myhface4_t :: myrule_t :: iso4) ? 
    this->myhface4(i)->subface4(this->twist(i) < 0 ? (9 - j + this->twist(i)) % 4 : (j + this->twist(i)) % 4) :
    (abort (), (const myhface4_t *)0) ;
}

template < class A > void HexaTop < A > :: splitISO8 () 
{
  int l = 1 + this->level () ;

  assert (_inner == 0 ) ;  
  {
    alucoord_t p[3] ;
    // calculate barycenter 
    TrilinearMapping::barycenter(
        this->myvertex(0)->Point(), this->myvertex(1)->Point(),
        this->myvertex(2)->Point(), this->myvertex(3)->Point(), this->myvertex(4)->Point(),
        this->myvertex(5)->Point(), this->myvertex(6)->Point(), this->myvertex(7)->Point(), 
        p) ;
    
    innervertex_t* v0 = static_cast<innervertex_t *> (this->myvertex (0));
    _inner = new inner_t (l, p[0], p[1], p[2], *v0 ) ;
    assert (_inner) ;
  }

  myvertex_t * fv0 = this->myhface4 (0)->subvertex (0) ;
  myvertex_t * fv1 = this->myhface4 (1)->subvertex (0) ;
  myvertex_t * fv2 = this->myhface4 (2)->subvertex (0) ;
  myvertex_t * fv3 = this->myhface4 (3)->subvertex (0) ;
  myvertex_t * fv4 = this->myhface4 (4)->subvertex (0) ;
  myvertex_t * fv5 = this->myhface4 (5)->subvertex (0) ;
  assert(fv0 && fv1 && fv2 && fv3 && fv4 && fv5) ;

#ifdef USE_MALLOC_AT_ONCE
  void* edgeMem[6] = {0,0,0,0,0,0};
  // allocate 6 * sizeof innerege_t 
  this->mallocAtOnce( sizeof(inneredge_t), edgeMem, 6 );
  inneredge_t * e0 = new (edgeMem[0]) inneredge_t (l, fv0, inVx()) ;
  inneredge_t * e1 = new (edgeMem[1]) inneredge_t (l, fv1, inVx()) ;
  inneredge_t * e2 = new (edgeMem[2]) inneredge_t (l, fv2, inVx()) ;
  inneredge_t * e3 = new (edgeMem[3]) inneredge_t (l, fv3, inVx()) ;
  inneredge_t * e4 = new (edgeMem[4]) inneredge_t (l, fv4, inVx()) ;
  inneredge_t * e5 = new (edgeMem[5]) inneredge_t (l, fv5, inVx()) ;
#else 
  inneredge_t * e0 = new inneredge_t (l, fv0, inVx()) ;
  inneredge_t * e1 = new inneredge_t (l, fv1, inVx()) ;
  inneredge_t * e2 = new inneredge_t (l, fv2, inVx()) ;
  inneredge_t * e3 = new inneredge_t (l, fv3, inVx()) ;
  inneredge_t * e4 = new inneredge_t (l, fv4, inVx()) ;
  inneredge_t * e5 = new inneredge_t (l, fv5, inVx()) ;
#endif

  assert(e0 && e1 && e2 && e3 && e4 && e5) ;
  e0->append(e1) ;
  e1->append(e2) ;
  e2->append(e3) ;
  e3->append(e4) ;
  e4->append(e5) ;

#ifdef USE_MALLOC_AT_ONCE
  void* faceMem[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  this->mallocAtOnce( sizeof(innerface_t), faceMem, 12 );
  innerface_t * f0 = new (faceMem[0]) innerface_t (l, this->subedge1 (2, 7), 0, e2, 0, e5, 1, this->subedge1 (5, 4), 1 ) ;
  innerface_t * f1 = new (faceMem[1]) innerface_t (l, this->subedge1(2, 5), 1, this->subedge1 (3, 7), 0, e3, 0, e2, 1 ) ;
  innerface_t * f2 = new (faceMem[2]) innerface_t (l, e3, 1, this->subedge1 (3, 5), 1, this->subedge1 (4, 7), 0, e4, 0 ) ;
  innerface_t * f3 = new (faceMem[3]) innerface_t (l, e5, 0, e4, 1, this->subedge1 (4, 5), 1, this->subedge1 (5, 6), 0 ) ;
  innerface_t * f4 = new (faceMem[4]) innerface_t (l, this->subedge1 (0, 7), 0, e0, 0, e2, 1, this->subedge1 (2, 4), 1 ) ;
  innerface_t * f5 = new (faceMem[5]) innerface_t (l, this->subedge1 (0, 5), 1, this->subedge1 (4, 4), 0, e4, 0, e0, 1 ) ; 
  innerface_t * f6 = new (faceMem[6]) innerface_t (l, e4, 1, this->subedge1 (4, 6), 1, this->subedge1 (1, 6), 0, e1, 0 ) ;
  innerface_t * f7 = new (faceMem[7]) innerface_t (l, e2, 0, e1, 1, this->subedge1 (1, 4), 1, this->subedge1 (2, 6), 0 ) ;
  innerface_t * f8 = new (faceMem[8]) innerface_t (l, this->subedge1 (0, 4), 0, e0, 0, e5, 1, this->subedge1 (5, 7), 1 ) ;
  innerface_t * f9 = new (faceMem[9]) innerface_t (l, this->subedge1 (0, 6), 1, this->subedge1 (3, 4), 0, e3, 0, e0, 1 ) ;
  innerface_t * f10 = new (faceMem[10]) innerface_t (l, e3, 1, this->subedge1 (3, 6), 1, this->subedge1 (1, 5), 0, e1, 0 ) ;
  innerface_t * f11 = new (faceMem[11]) innerface_t (l, e5, 0, e1, 1, this->subedge1 (1, 7), 1, this->subedge1 (5, 5), 0 ) ;
#else 
  innerface_t * f0 = new innerface_t (l, this->subedge1 (2, 7), 0, e2, 0, e5, 1, this->subedge1 (5, 4), 1) ;
  innerface_t * f1 = new innerface_t (l, this->subedge1(2, 5), 1, this->subedge1 (3, 7), 0, e3, 0, e2, 1) ;
  innerface_t * f2 = new innerface_t (l, e3, 1, this->subedge1 (3, 5), 1, this->subedge1 (4, 7), 0, e4, 0 ) ;
  innerface_t * f3 = new innerface_t (l, e5, 0, e4, 1, this->subedge1 (4, 5), 1, this->subedge1 (5, 6), 0 ) ;
  innerface_t * f4 = new innerface_t (l, this->subedge1 (0, 7), 0, e0, 0, e2, 1, this->subedge1 (2, 4), 1 ) ;
  innerface_t * f5 = new innerface_t (l, this->subedge1 (0, 5), 1, this->subedge1 (4, 4), 0, e4, 0, e0, 1 ) ; 
  innerface_t * f6 = new innerface_t (l, e4, 1, this->subedge1 (4, 6), 1, this->subedge1 (1, 6), 0, e1, 0 ) ;
  innerface_t * f7 = new innerface_t (l, e2, 0, e1, 1, this->subedge1 (1, 4), 1, this->subedge1 (2, 6), 0 ) ;
  innerface_t * f8 = new innerface_t (l, this->subedge1 (0, 4), 0, e0, 0, e5, 1, this->subedge1 (5, 7), 1 ) ;
  innerface_t * f9 = new innerface_t (l, this->subedge1 (0, 6), 1, this->subedge1 (3, 4), 0, e3, 0, e0, 1 ) ;
  innerface_t * f10 = new innerface_t (l, e3, 1, this->subedge1 (3, 6), 1, this->subedge1 (1, 5), 0, e1, 0 ) ;
  innerface_t * f11 = new innerface_t (l, e5, 0, e1, 1, this->subedge1 (1, 7), 1, this->subedge1 (5, 5), 0 ) ;
#endif

  assert(f0 && f1 && f2 && f3 && f4 && f5 && f6 && f7 && f8 && f9 && f10 && f11) ;
  f0->append(f1) ;
  f1->append(f2) ;
  f2->append(f3) ;
  f3->append(f4) ;
  f4->append(f5) ;
  f5->append(f6) ;
  f6->append(f7) ;
  f7->append(f8) ;
  f8->append(f9) ;
  f9->append(f10) ;
  f10->append(f11) ;

  // calculate child volume which is volume divided by 8 
  double childVolume = 0.125 * _volume;

  // only check for affine faces 
  // for other it does not matter 
  if( _affine ) 
  {
    // if vertex projection is available
    // then set affine to false to invoke volume calculation  
    for( int i=0; i<6; ++i) 
    {
      if( this->myneighbour( i ).first->hasVertexProjection() )
      {
        _affine = false ; 
        break ;
      }
    }
  }

#ifdef USE_MALLOC_AT_ONCE
  void* hexaMem[8] = {0,0,0,0,0,0,0,0};
  this->mallocAtOnce( sizeof(innerhexa_t), hexaMem, 8 );

  innerhexa_t * h0 = new (hexaMem[0]) innerhexa_t (l, this->subface4 (0, 0), this->twist (0), f0, 0, this->subface4 (2, 0), this->twist (2), f4, 0, f8, -4, this->subface4 (5, 0), this->twist (5) , this, 0, childVolume) ;
  innerhexa_t * h1 = new (hexaMem[1]) innerhexa_t (l, this->subface4 (0, 3), this->twist (0), f1, 0, this->subface4 (2, 1), this->twist (2), this->subface4 (3, 0), this->twist (3), f9, -4, f4, -1, this, 1, childVolume) ;
  innerhexa_t * h2 = new (hexaMem[2]) innerhexa_t (l, this->subface4 (0, 2), this->twist (0), f2, 0,f9, 0, subface4 (3, 1), this->twist (3), this->subface4 (4, 0), this->twist (4), f5, -1        , this, 2, childVolume) ;
  innerhexa_t * h3 = new (hexaMem[3]) innerhexa_t (l, this->subface4 (0, 1), this->twist (0), f3, 0, f8, 0, f5, 0, this->subface4(4, 1), this->twist (4), this->subface4(5, 3), this->twist (5)    , this, 3, childVolume) ;
  innerhexa_t * h4 = new (hexaMem[4]) innerhexa_t (l, f0, -1, this->subface4(1, 0), this->twist (1), this->subface4(2, 3), this->twist (2), f7, 0, f11, -4, this->subface4(5, 1), this->twist (5)  , this, 4, childVolume) ;
  innerhexa_t * h5 = new (hexaMem[5]) innerhexa_t (l, f1, -1, this->subface4(1, 1), this->twist (1), this->subface4(2, 2), this->twist (2), this->subface4(3, 3), this->twist (3), f10, -4, f7, -1 , this, 5, childVolume) ;
  innerhexa_t * h6 = new (hexaMem[6]) innerhexa_t (l, f2, -1, this->subface4(1, 2), this->twist (1), f10, 0, this->subface4(3, 2), this->twist (3), this->subface4(4, 3), this->twist (4), f6, -1  , this, 6, childVolume) ;
  innerhexa_t * h7 = new (hexaMem[7]) innerhexa_t (l, f3, -1, this->subface4(1, 3), this->twist (1), f11, 0, f6, 0, this->subface4(4, 2), this->twist (4), this->subface4(5, 2), this->twist (5)   , this, 7, childVolume) ;
#else 
  innerhexa_t * h0 = new innerhexa_t (l, this->subface4 (0, 0), this->twist (0), f0, 0, this->subface4 (2, 0), this->twist (2), f4, 0, f8, -4, this->subface4 (5, 0), this->twist (5) , this, 0, childVolume) ;
  innerhexa_t * h1 = new innerhexa_t (l, this->subface4 (0, 3), this->twist (0), f1, 0, this->subface4 (2, 1), this->twist (2), this->subface4 (3, 0), this->twist (3), f9, -4, f4, -1, this, 1, childVolume) ;
  innerhexa_t * h2 = new innerhexa_t (l, this->subface4 (0, 2), this->twist (0), f2, 0,f9, 0, subface4 (3, 1), this->twist (3), this->subface4 (4, 0), this->twist (4), f5, -1        , this, 2, childVolume) ;
  innerhexa_t * h3 = new innerhexa_t (l, this->subface4 (0, 1), this->twist (0), f3, 0, f8, 0, f5, 0, this->subface4(4, 1), this->twist (4), this->subface4(5, 3), this->twist (5)    , this, 3, childVolume) ;
  innerhexa_t * h4 = new innerhexa_t (l, f0, -1, this->subface4(1, 0), this->twist (1), this->subface4(2, 3), this->twist (2), f7, 0, f11, -4, this->subface4(5, 1), this->twist (5)  , this, 4, childVolume) ;
  innerhexa_t * h5 = new innerhexa_t (l, f1, -1, this->subface4(1, 1), this->twist (1), this->subface4(2, 2), this->twist (2), this->subface4(3, 3), this->twist (3), f10, -4, f7, -1 , this, 5, childVolume) ;
  innerhexa_t * h6 = new innerhexa_t (l, f2, -1, this->subface4(1, 2), this->twist (1), f10, 0, this->subface4(3, 2), this->twist (3), this->subface4(4, 3), this->twist (4), f6, -1  , this, 6, childVolume) ;
  innerhexa_t * h7 = new innerhexa_t (l, f3, -1, this->subface4(1, 3), this->twist (1), f11, 0, f6, 0, this->subface4(4, 2), this->twist (4), this->subface4(5, 2), this->twist (5)   , this, 7, childVolume) ;
#endif

  assert(h0 && h1 && h2 && h3 && h4 && h5 && h6 && h7) ;
  h0->append(h1) ;
  h1->append(h2) ;
  h2->append(h3) ;
  h3->append(h4) ;
  h4->append(h5) ;
  h5->append(h6) ;
  h6->append(h7) ;

  // inner edge 
  _inner->store( e0 );
  // inne face 
  _inner->store( f0 );
  // down ptr 
  _inner->store( h0 );
  _rule = myrule_t :: iso8 ;
  this->detachleafs();
  return ;
}

template < class A > void HexaTop < A > :: refineImmediate (myrule_t r) 
{
  assert (getrule () == myrule_t :: nosplit) ;
  switch(r) {
    case myrule_t :: iso8 :
    
  // Das refineImmediate (..) auf allen Fl"achen wird vom Hexa :: refine (..)
  // zwar nicht ben"otigt, da schliesslich alle Fl"achen sauber sind wenn
  // "uberall hface4 :: refine (..) true geliefert hat, wohl aber z.B. von
  // restore () oder abgeleiteten Funktionen die eine direkte Verfeinerung
  // erzwingen m"ussen und d"urfen.
    
      {
        typedef typename myhface4_t :: myrule_t myhface4rule_t;
        for (int i = 0 ; i < 6 ; i ++)
        this->myhface4 (i)->refineImmediate (myhface4rule_t (myhface4_t :: myrule_t :: iso4).rotate (this->twist (i))) ; }
      splitISO8 () ;
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

template < class A > bool HexaTop < A > :: refine () {
  myrule_t r = _req ;
  if (r != myrule_t :: crs && r != myrule_t :: nosplit) {
    if (r != getrule ()) {
      assert (getrule () == myrule_t :: nosplit) ;
      _req = myrule_t :: nosplit ;
      switch (r) {
        case myrule_t :: crs :
        case myrule_t :: nosplit :
          return true ;
        case myrule_t :: iso8 :
    {
      typedef typename myhface4_t :: myrule_t myhface4rule_t;
      for (int i = 0 ; i < 6 ; i ++ )
            if (!this->myhface4 (i)->refine (myhface4rule_t (myhface4_t :: myrule_t :: iso4)
              .rotate (this->twist (i)), this->twist (i))) return false ;
          }
          refineImmediate (r) ;
          return true ;
        default :
          cerr << "**WARNUNG (FEHLER IGNORIERT) falsche Verfeinerungsregel [" << getrule () ;
          cerr << "] (ignoriert) in " << __FILE__ << " " << __LINE__ << endl ;
          return false ;
      }
    }
  }
  return true ;
}

template < class A > bool HexaTop < A > :: refineBalance (balrule_t r, int fce) {
  assert (r == balrule_t :: iso4) ;
  if (getrule () == myrule_t :: nosplit) {
    if (! this->myhface4 (fce)->leaf ()) {
      for (int i = 0 ; i < 6 ; i ++)
        if (i != fce)
          if (!this->myhface4 (i)->refine (balrule_t (balrule_t :: iso4).rotate (this->twist (i)), this->twist (i))) 
      return false ;
      _req = myrule_t :: nosplit ;
      refineImmediate (myrule_t :: iso8) ;
    }
  }
  return true ;
}

template < class A > bool HexaTop < A > :: coarse () {
  if (this->leaf ()) 
  {
    assert (_req == myrule_t :: nosplit || _req == myrule_t :: crs) ;
    myrule_t w = _req ;
    _req = myrule_t :: nosplit ;
    if (w != myrule_t :: crs)
    {
      return false ;
    }
    for (int i = 0 ; i < 6 ; ++i) 
    {
      if (!this->myhface4 (i)->leaf ()) return false ;
    }
    return true ;
  } 
  else 
  {
    assert (_req == myrule_t :: nosplit) ;
    bool x = true ;
    {
      for (innerhexa_t * h = dwnPtr() ; h ; h = h->next ()) x &= h->coarse () ; 
    }
    if (x) 
    {
      this->preCoarsening () ;
      this->attachleafs();
      delete _inner ; 
      _inner = 0 ;

      _rule = myrule_t :: nosplit ;
      {
        for (int i = 0 ; i < 6 ; ++i) 
        {
          this->myneighbour (i).first->bndNotifyCoarsen () ;
          this->myhface4 (i)->coarse () ;
        }
      }
      return false ;
    }
  }
  return false ;
}

template < class A > bool HexaTop < A > :: bndNotifyCoarsen () {
  return true ;
}

template < class A > void HexaTop < A > :: backupCMode (ostream & os) const {

  // Das backup im alten Stil, d.h. levelweise die Verfeinerungsregeln
  // vom Gitter runterschreiben. Diese Technik wird nur f"ur das backup
  // noch unterst"utzt, um die Daten mit "alteren Konstruktionen visual.
  // zu k"onnen.

  os << getrule () << " " ;
  return ;
}

template < class A > void HexaTop < A > :: backupIndex (ostream & os) const {
  os.write(((const char *) & this->_idx ), sizeof(int));
  for (const innerhexa_t* c = down(); c; c = c->next()) {
    c->backupIndex(os);
  }
  return;
}

template < class A > void HexaTop < A > :: backup (ostream & os) const 
{
  doBackup( os );
}
template < class A > void HexaTop < A > :: backup (ObjectStream& os) const 
{
  doBackup( os );
}

template < class A > template <class OutStream_t>
void HexaTop < A > :: doBackup (OutStream_t& os) const 
{
  os.put ((char) getrule ()) ;
  {for (const inneredge_t * e = innerHedge () ; e ; e = e->next ()) e->backup (os) ; }
  {for (const innerface_t * f = innerHface () ; f ; f = f->next ()) f->backup (os) ; }
  {for (const innerhexa_t * c = dwnPtr() ; c ; c = c->next ()) c->backup (os) ; }
  return ;
}

template < class A > void HexaTop < A > :: 
restoreIndex (istream & is, vector<bool> (&isHole) [4]) 
{
  // free index from constructor
  // indexManager is cleared from outside 
  is.read ( ((char *) &(this->_idx) ), sizeof(int) );

  // mark this element a non hole 
  typedef typename Gitter :: Geometric :: BuilderIF BuilderIF;

  // make sure sizes match 
  assert( this->getIndex() < (int) isHole[BuilderIF::IM_Elements].size() );
  // set entry to false, because this is not a hole 
  isHole[BuilderIF :: IM_Elements][this->getIndex()] = false;
  
  {for (innerhexa_t * c = dwnPtr() ; c ; c = c->next ()) c->restoreIndex (is, isHole ) ; }
  return;
}

template < class A > void HexaTop < A > :: restore (istream & is) 
{
  doRestore( is );
}
template < class A > void HexaTop < A > :: restore (ObjectStream& is) 
{
  doRestore( is );
}

template < class A > template <class InStream_t>
void HexaTop < A > :: doRestore (InStream_t & is) 
{
  // restore () stellt den Elmentbaum aus der Verfeinerungs
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
  
    for (int i = 0 ; i < 6 ; i ++) {
      myhface4_t & f (*(this->myhface4 (i))) ;
      if (!f.leaf ()) {
        for (int j = 0 ; j < 4 ; j ++) f.subface4 (j)->nb.complete (f.nb) ;
      }
    }
  } 
  else 
  {
    request (r) ;
    refine () ;
    assert (getrule() == r) ;
    {for (inneredge_t * e = innerHedge () ; e ; e = e->next ()) e->restore (is) ; }
    {for (innerface_t * f = innerHface () ; f ; f = f->next ()) f->restore (is) ; }
    {for (innerhexa_t * c = dwnPtr () ; c ; c = c->next ()) c->restore (is) ; }
  }
  return ;
}
// ######                                                          #       #######
// #     #  ######  #####      #     ####   #####      #     ####  #    #     #
// #     #  #       #    #     #    #    #  #    #     #    #    # #    #     #
// ######   #####   #    #     #    #    #  #    #     #    #      #    #     #
// #        #       #####      #    #    #  #    #     #    #      #######    #
// #        #       #   #      #    #    #  #    #     #    #    #      #     #
// #        ######  #    #     #     ####   #####      #     ####       #     #

   
template < class A >  Periodic4Top < A > :: Periodic4Top (int l, myhface4_t * f0, int t0,
  myhface4_t * f1, int t1) 
  : A (f0, t0, f1, t1)
  , _dwn (0), _bbb (0), _up(0)
  , _lvl (l) 
  , _nChild (0)
  , _rule (myrule_t :: nosplit)
{
  return ;
}

template < class A >  Periodic4Top < A > :: Periodic4Top (int l, myhface4_t * f0, 
    int t0, myhface4_t * f1, int t1, innerperiodic4_t * up, int nChild )
: A (f0, t0, f1, t1)
  , _dwn (0), _bbb (0), _up(up)
  , _lvl (l)
  , _nChild (nChild) 
  , _rule (myrule_t :: nosplit)
{ 
  return ;
}

template < class A >  Periodic4Top < A > :: ~Periodic4Top () {  
  if (_bbb) delete _bbb ;
  if (_dwn) delete _dwn ;
  return ;
}

template < class A > void Periodic4Top < A > :: splitGhosts () {
  for (int g = 0; g < 2; ++g) {
    ghostpair_STI ghostpair = _ghostPair[g]; 
    if (ghostpair.first) {
      GhostElement_t & ghost = static_cast<GhostElement_t &> (*ghostpair.first);
      ghost.request( Gitter::Geometric::HexaRule::iso8 );
      assert( Gitter::Geometric::HexaRule::iso8 == ghost.requestrule ());
      ghost.refine();
    }
  }
}

template < class A > void Periodic4Top < A > :: coarseGhosts () {
  for (int g = 0; g < 2; ++g) {
    if(_ghostPair[g].first) {
      GhostElement_t & ghost = static_cast<GhostElement_t &> (*(_ghostPair[g].first));
      ghost.request( Gitter::Geometric::HexaRule::crs );
      assert( Gitter::Geometric::HexaRule::crs == ghost.requestrule ());
      ghost.coarse();
      assert( ghost.leaf() );
    }
  }
}

template < class A > typename Periodic4Top < A > :: myhedge1_t * Periodic4Top < A > :: subedge1 (int i, int j) {  
  assert (getrule () == myrule_t :: iso4) ;
  return (j < 4) ? ((this->twist (i) < 0) ? this->myhface4 (i)->myhedge1 ((8 - j + this->twist (i)) % 4) :  // aus dem Hexaeder
    this->myhface4 (i)->myhedge1 ((j + this->twist (i)) % 4)) : 
    ((this->twist (i) < 0) ? this->myhface4 (i)->subedge1 ((12 - j + this->twist (i)) % 4) : 
    this->myhface4 (i)->subedge1 ((j + this->twist (i)) % 4)) ;
}

template < class A > const typename Periodic4Top < A > :: myhedge1_t * Periodic4Top < A > :: subedge1 (int i, int j) const { 
  return ((Periodic4Top < A > *)this)->subedge1 (i,j) ;
}

template < class A > typename Periodic4Top < A > ::  myhface4_t * Periodic4Top < A > :: subface4 (int i, int j) { 
  return (this->myhface4(i)->getrule() == myhface4_t :: myrule_t :: iso4) ? 
  this->myhface4(i)->subface4(this->twist(i) < 0 ? (9 - j + this->twist(i)) % 4 : (j + this->twist(i)) % 4) : // Zeile aus dem Hexaeder
  (abort (), (myhface4_t *)0) ;
}

template < class A > const typename Periodic4Top < A > ::  myhface4_t * Periodic4Top < A > :: subface4 (int i, int j) const {
  return ((Periodic4Top < A > *)this)->subface4 (i,j) ;
}

template < class A > void Periodic4Top < A > :: splitISO4 () 
{  
  assert (_dwn == 0) ;

  // get the childs 
  typedef Gitter :: ghostpair_STI ghostpair_STI;
  typedef typename Gitter :: Geometric :: hexa_GEO  hexa_GEO;
  typedef typename Gitter :: Geometric :: hface4_GEO hface4_GEO;
  
  hexa_GEO *(ghchild)[4] = {0,0,0,0};
  
  // refine ghost element 
  this->splitGhosts();
  
  for (int g = 0; g < 2; g++) {  
    ghostpair_STI ghostpair = _ghostPair[g];
  
    hexa_GEO * gh = dynamic_cast<hexa_GEO *> ((ghostpair).first);
  
    int gFace[4] = { -1,-1,-1,-1 };
    if(gh)
    {
      int gFaceNum = ghostpair.second; 
      assert( gFaceNum >= 0 );
      assert( gFaceNum < 6 );
      hface4_GEO * face = gh->myhface4( gFaceNum );
      assert( face );
    
      face = face->down();
      for(int i=0; i<4; i++)
      {
        assert(face);
        typedef pair < Gitter :: Geometric :: hasFace4 *, int > neigh_t;
        neigh_t neighbour = face->nb.front();
        hexa_GEO * ghch = static_cast<hexa_GEO *> (neighbour.first);
        if(ghch)
        { 
          if(ghch->up() != gh) 
          {
            neighbour = face->nb.rear();
            ghch = static_cast<hexa_GEO *> (neighbour.first);
          }
        }
        else 
        { 
          neighbour = face->nb.rear();
          ghch = static_cast<hexa_GEO *> (neighbour.first);
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

  int l = 1 + this->level () ;
  innerperiodic4_t * p0 = new innerperiodic4_t (l, this->subface4 (0,0), this->twist (0), this->subface4 (1,0), this->twist (1), this, 0) ;
  innerperiodic4_t * p1 = new innerperiodic4_t (l, this->subface4 (0,1), this->twist (0), this->subface4 (1,3), this->twist (1), this, 1) ;
  innerperiodic4_t * p2 = new innerperiodic4_t (l, this->subface4 (0,2), this->twist (0), this->subface4 (1,2), this->twist (1), this, 2) ;
  innerperiodic4_t * p3 = new innerperiodic4_t (l, this->subface4 (0,3), this->twist (0), this->subface4 (1,1), this->twist (1), this, 3) ;
  assert (p0 && p1 && p2 && p3) ;
  p0->append(p1) ;
  p1->append(p2) ;
  p2->append(p3) ;
  _dwn = p0 ;
  _rule = myrule_t :: iso4 ;
  return ;
}

template < class A > void Periodic4Top < A > :: refineImmediate (myrule_t r) {

  // Die Methode wird nur vom restore () und vom refineBalance () auf-
  // gerufen und geht davon aus, dass das betroffene Element noch nicht
  // verfeinert ist -> ist ein Blatt der Hierarchie.

  assert (this->leaf()) ;
  switch (r) {
    case myrule_t :: iso4 :
    
  // Das refineImmediate (..) auf allen Fl"achen wird vom periodic4 :: refine (..)
  // zwar nicht ben"otigt, da schliesslich alle Fl"achen sauber sind, wenn
  // "uberall hface4 :: refine (..) true geliefert hat, wohl aber z.B. von
  // restore () oder abgeleiteten Funktionen, die eine direkte Verfeinerung
  // erzwingen m"ussen und d"urfen.
    
      typedef typename myhface4_t :: myrule_t myhface4rule_t;
      this->myhface4 (0)->refineImmediate (myhface4rule_t (r).rotate (this->twist (0))) ;
      this->myhface4 (1)->refineImmediate (myhface4rule_t (r).rotate (this->twist (1))) ;
      splitISO4 () ;
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

template < class A > bool Periodic4Top < A > :: refineBalance (balrule_t r, int fce) {  
  if (r != balrule_t :: iso4) {
    cerr << "**WARNUNG (IGNORIERT) in Periodic4Top < A > :: refineBalance (..) nachschauen, Datei " 
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
  
    typedef typename myhface4_t :: myrule_t myhface4rule_t;
    int opp = fce == 0 ? 1 : 0 ;
    if (this->myhface4 (opp)->refine (myhface4rule_t (r).rotate (this->twist (opp)), this->twist (opp))) {
      refineImmediate (r) ;
      return true ;
    } else {
      return false ;
    }
  }
}

template < class A > bool Periodic4Top < A > :: coarse () { 

  // Das Vergr"obern geschieht auch passiv, sobald ein anliegendes Element
  // vergr"obert wird durch den Aufruf von "bndNotifyCoarsen ()" s.u.

  bndNotifyCoarsen () ;
  return false ;
}

template < class A > bool Periodic4Top < A > :: bndNotifyCoarsen () {

  // Wie beim Randelement auch: Die Vergr"oberung eines anliegenden Elements
  // l"ost einen Vorgang aus, der feststellt ob das periodische RE ebenfalls
  // vergr"obert werden soll.
  
  innerperiodic4_t * p = down () ;
  if (!p) return false ;
  bool x = true ;
  do {

  // Falls p kein Blatt der Hierarchie ist, 
  // die Vergr"oberungsm"oglichkeit weitergeben.

    if (!p->leaf ()) p->coarse () ;

  // F"ur die hintere und vordere Fl"ache feststellen, ob
  // der Referenzenz"ahler mehr als einen Eintrag ergibt.

    if (p->myhface4 (0)->ref > 1) (x = false) ;
    if (p->myhface4 (1)->ref > 1) (x = false) ;

  } while ( (p = p->next ()) ) ;
  if (x) {
  
  // Falls keine Fl"achen anliegen, die auf Kinder oder Kindes- 
  // mit mehr als einer Referenz f"uhren, ist sicher, dass das
  // Bezugsrandelement zwischen zwei 'relativ groben' Elementen
  // liegt. Somit kann es vergr"obert werden.
  
    this->preCoarsening () ;
    
    // coarse ghost elements 
    this->coarseGhosts() ;

    delete _dwn ;
    _dwn = 0 ;
    _rule = myrule_t :: nosplit ;
    this->myhface4 (0)->coarse () ;
    this->myhface4 (1)->coarse () ;
  }
  return x ;
}

template < class A > void Periodic4Top < A > :: backupCMode (ostream & os) const {

  // Das backup im alten Stil, d.h. levelweise die Verfeinerungsregeln
  // vom Gitter runterschreiben. Diese Technik wird nur f"ur das backup
  // noch unterst"utzt, um die Daten mit "alteren Konstruktionen visual.
  // zu k"onnen.
  
  os << getrule () << " " ;
  return ;
}

template < class A > void Periodic4Top < A > :: backup (ostream & os) const 
{
  doBackup( os );
}
template < class A > void Periodic4Top < A > :: backup (ObjectStream& os) const 
{
  doBackup( os );
}
template < class A > template <class OutStream_t>
void Periodic4Top < A > :: doBackup (OutStream_t& os) const 
{
  os.put ((char) getrule ()) ;
  {for (const innerperiodic4_t * c = down () ; c ; c = c->next ()) c->backup (os) ; }
  return ;
}

template < class A > void Periodic4Top < A > :: restore (istream & is) 
{
  doRestore( is );
}

template < class A > void Periodic4Top < A > :: restore (ObjectStream& is) 
{
  doRestore( is );
}

template < class A > template <class InStream_t>
void Periodic4Top < A > :: doRestore (InStream_t& is) 
{
  myrule_t r ((char) is.get ()) ;
  assert(getrule () == myrule_t :: nosplit) ; // Testen auf unverfeinerten Zustand
  if (r == myrule_t :: nosplit) {
    for (int i = 0 ; i < 2 ; i ++) {
      myhface4_t & f (*(this->myhface4 (i))) ;
      if (!f.leaf ()) {
        switch (f.getrule ()) {
    case balrule_t :: iso4 :
            {for (int j = 0 ; j < 4 ; j ++) f.subface4 (j)->nb.complete (f.nb) ;}
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
  else 
  {
    refineImmediate (r) ;
    assert (getrule() == r) ;
    {for (innerperiodic4_t * c = down () ; c ; c = c->next ()) c->restore (is) ; }
  }
  return ;
}


// template instantiation 
#ifndef GITTER_HEXA_TOP_PLL_H_INCLUDED
template class Hedge1Top< GitterBasis :: Objects :: Hedge1Empty > ;
template class Hface4Top< GitterBasis :: Objects :: Hface4Empty > ;
template class Hbnd4Top< GitterBasis :: Objects :: Hbnd4Default > ;
template class HexaTop < GitterBasis :: Objects :: HexaEmpty > ;
template class Periodic4Top < GitterBasis :: Objects :: Periodic4Empty > ;
#endif
