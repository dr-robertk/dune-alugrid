// (c) mario ohlberger, 1998
// modifications for dune interface
// (c) Robert Kloefkorn 2004 - 2005
#ifndef GITTER_TETRATOP_H_INCLUDED
#define GITTER_TETRATOP_H_INCLUDED

#include "gitter_sti.h"
#include "gitter_hexa_top.h"
#include "mappings.hh"

namespace ALUGrid
{

  // --checkFace
  template < class A >
  inline bool checkFace ( const A* fce, const int child )
  {
    A* face = const_cast< A* > (fce);
    const int vxs[ 3 ][ 2 ] = { { 0, 1 }, { 0, 2 }, { 1,2 } };
    bool found = true;
    for( int i=0; i<3; ++i )
    {
      // check vertices of the face
      for( int j=0; j<2; ++j )
      {
        bool foundVx = false;
        for( int e=0; e<2; ++e )
        {
          if( face->myvertex( vxs[ i ][ j ] ) == face->myhedge( i )->myvertex( e ) )
            foundVx = true;
        }
        if( ! foundVx )
        {
          std::cout << "Edge inconsistency: " << face << std::endl;
          for( int e=0; e<3; ++e )
          {
            std::cout << "edge " << face->myhedge( e )->myvertex( 0 ) << " " <<
              face->myhedge( e )->myvertex( 1 ) << std::endl;
          }
          alugrid_assert ( false );
        }
      }

//TODO write a check here
      for( int j=1; j<3; ++j )
      {
        int f = (i+j)%3;
        if( face->myhedge( i )->getIndex() == face->myhedge( f )->getIndex() )
        {
          std::cout << "Edge " << i << "  " << face->myhedge( i ) << std::endl;
          std::cout << "Edge " << f << "  " << face->myhedge( f ) << std::endl;
          alugrid_assert ( false );
        }
      }
    }
    return found;
  }

  template < class A > class Hface3Top : public A
  {
    public :
      using A :: myhedge;
      using A :: myvertex;

      typedef Hface3Top < A >             innerface_t;
      typedef typename A :: inneredge_t   inneredge_t;
      typedef typename A :: innervertex_t innervertex_t;
      typedef typename A :: myhedge_t     myhedge_t;
      typedef typename A :: myvertex_t    myvertex_t;
      typedef typename A :: myrule_t      myrule_t;
      typedef InnerStorage< InnerEdgeStorage< innerface_t , false > > inner_t;

      typedef std::pair< myhedge_t*, myhedge_t* > edgepair_t;

    private :
      innerface_t * _bbb;
      inner_t  * _inner;

      const unsigned char _lvl;
      const signed char _nChild;
      myrule_t _rule;

    protected:
      // we need this because TetraTop needs access to this indexManager
      inline IndexManagerType & indexManager () {
        return this->myvertex(0)->indexManagerStorage().get( IndexManagerStorageType :: IM_Faces ); }

    private:
      inline myhedge_t * subedge (int,int);
      inline const myhedge_t * subedge (int,int) const;
      void split_e01 ();
      void split_e12 ();
      void split_e02 ();
      void split_iso4 ();

    public :
      // constructor for macro elements
      inline Hface3Top (int,myhedge_t *,myhedge_t *,myhedge_t * );
      // constructor for refined elements
      inline Hface3Top (int,myhedge_t *,myhedge_t *,myhedge_t *, int nChild );
      virtual inline ~Hface3Top ();
      innervertex_t * subvertex (int);
      const innervertex_t * subvertex (int) const;
      inneredge_t * subedge (int);
      const inneredge_t * subedge (int) const;
      innerface_t * subface (int);
      const innerface_t * subface (int) const;
      inline int level () const;
      inline int nChild () const;
      innervertex_t * innerVertex ();
      const innervertex_t * innerVertex () const;
      inneredge_t * innerHedge ();
      const inneredge_t * innerHedge () const;
      innerface_t * down ();
      const innerface_t * down () const;
      innerface_t * next ();
      const innerface_t * next () const;
      void append (innerface_t * f);
    public:
      virtual myrule_t getrule () const;
      virtual bool refine (myrule_t,bool);
      virtual void refineImmediate (myrule_t);
      virtual bool coarse ();
    public :
      virtual void backup (std::ostream &) const;
      virtual void restore (std::istream &);

      virtual void backup (ObjectStream &) const;
      virtual void restore (ObjectStream &);

    protected:
      myvertex_t* vertexNotOnSplitEdge( const int );
      edgepair_t subEdges( myhedge_t* , const myvertex_t* , const myvertex_t*  );

      // non-virtual methods of down and innerVertex
      innerface_t* dwnPtr();
      const innerface_t* dwnPtr() const;
      inneredge_t* inEd();
      const inneredge_t* inEd() const;

      template <class OutStream_t>
      void doBackup(OutStream_t &) const;

      template <class InStream_t>
      void doRestore(InStream_t &);
  };


  template < class A > class Hbnd3Top : public A {
    public:
      using A :: isRear;
      using A :: subface;
      using A :: myhface;

    protected :
      typedef Hbnd3Top < A >              innerbndseg_t;
      typedef typename A :: myhface_t     myhface_t;
      typedef typename A :: balrule_t     balrule_t;
      typedef typename A :: myrule_t      myrule_t;
      typedef typename A :: bnd_t         bnd_t;

      bool refineLikeElement (balrule_t);
      inline void append (innerbndseg_t *);

      // we need access to the indexManager
      inline IndexManagerType & indexManager () {
        return  myhface(0)->myvertex(0)->indexManagerStorage().get( IndexManagerStorageType :: IM_Bnd );
      }

    private :
      innerbndseg_t * _bbb, * _dwn , * _up;  // 24 bytes

      int _segmentId; // segment index of macro face
      const bnd_t _bt; // type of boundary   // 32 bytes

      using A :: _lvl; // declared in hbndseg3_GEO

      // unsigned char _lvl;                    // 40 bytes

      void split_bisection ();
      void split_iso4 ();
      inline bool coarse ();

    public:
      // constructor for serial macro boundary elements
      inline Hbnd3Top (int,myhface_t *,int, const bnd_t b );
      // constructor for children
      inline Hbnd3Top (int, myhface_t *,int,
                       innerbndseg_t * up, const bnd_t b,
                       typename Gitter::helement_STI * gh, int gFace );

      inline virtual ~Hbnd3Top ();
      using A :: refineBalance;
      bool refineBalance (balrule_t,int);
      bool bndNotifyCoarsen ();
      void restoreFollowFace ();
      inline int level () const;
      inline int segmentId () const;
      inline innerbndseg_t * next ();
      inline innerbndseg_t * down ();
      inline const innerbndseg_t * next () const;
      inline const innerbndseg_t * down () const;

      // for dune
      inline innerbndseg_t * up ();
      inline const innerbndseg_t * up () const;

      inline bnd_t bndtype () const { return _bt; }

    protected:
      // set boundary id for all item connected to this hbnd
      void setBoundaryId( const int id );
  };

  template < class A > class TetraTop : public A
  {
    public :
      using A :: isRear;
      using A :: myhface;
      using A :: myvertex;
      using A :: myGrid;
      using A :: nEdges;
      using A :: nFaces;
      using A :: myhedge;

      typedef TetraTop < A >    innertetra_t ;
      typedef typename A :: innervertex_t innervertex_t;
      typedef typename A :: inneredge_t   inneredge_t;
      typedef typename A :: innerface_t   innerface_t;
      typedef Gitter :: Geometric :: VertexGeo  myvertex_t;
      typedef typename A :: myhedge_t    myhedge_t;
      typedef typename A :: myhface_t    myhface_t;
      typedef typename A :: myrule_t      myrule_t;
      typedef typename A :: balrule_t     balrule_t;
      typedef std::pair< myhface_t *, myhface_t * > facepair_t;

      typedef InnerStorage< InnerFaceStorage< innertetra_t , false > > inner_t;
      typedef typename myhface_t :: myrule_t face3rule_t;

    protected:
      struct BisectionInfo
      {
        struct CallSplitIF
        {
          virtual ~CallSplitIF () {}
          virtual void splitEdge( innertetra_t* tetra ) const = 0;
        };

        template < typename myrule_t :: rule_enum rule >
        struct CallSplitImpl : public CallSplitIF
        {
          virtual ~CallSplitImpl () {}
          virtual void splitEdge( innertetra_t* tetra ) const
          {
            if( rule == myrule_t :: e01 )
              tetra->split_e01();
            else if ( rule == myrule_t :: e12 )
              tetra->split_e12();
            else if ( rule == myrule_t :: e02 )
              tetra->split_e02();
            else if ( rule == myrule_t :: e23 )
              tetra->split_e23();
            else if ( rule == myrule_t :: e03 )
              tetra->split_e03();
            else if ( rule == myrule_t :: e13 )
              tetra->split_e13();
            else
            {
              std::cerr << "ERROR (FATAL): Wrong refinement rule." << std::endl;
              abort();
            }
          }
        };

        const CallSplitIF* _caller;
        unsigned char _faces[ 2 ];
        face3rule_t _faceRules[ 2 ];

      private:
        // constructor
        BisectionInfo( myrule_t rule );
        // no copying
        BisectionInfo( const BisectionInfo& );

      public:
        ~BisectionInfo() { delete _caller; }

        static const BisectionInfo& instance( const myrule_t& rule )
        {
          alugrid_assert ( rule == myrule_t :: e01 ||
                  rule == myrule_t :: e12 ||
                  rule == myrule_t :: e02 ||
                  rule == myrule_t :: e23 ||
                  rule == myrule_t :: e03 ||
                  rule == myrule_t :: e13  );

          static const BisectionInfo bisectionInfo[ 6 ] = {
              BisectionInfo( myrule_t :: e01 ),
              BisectionInfo( myrule_t :: e02 ),
              BisectionInfo( myrule_t :: e03 ),
              BisectionInfo( myrule_t :: e12 ),
              BisectionInfo( myrule_t :: e13 ),
              BisectionInfo( myrule_t :: e23 ) };
          return bisectionInfo[ int(rule) - 2 ];
        }


        static bool refineFaces( innertetra_t* tetra, const myrule_t& rule )
        {
          const BisectionInfo& info = instance( rule );
          for( int i=0; i<2; ++i )
          {
            myhface_t* face = tetra->myhface( info._faces[ i ] );

            const face3rule_t faceRule = info._faceRules[i];

            //std::cout << "Calculated Face  Rule: "<< faceRule << " with TetraRule: " << rule <<  " for " << tetra << " and " << face << std::endl;

            // check refinement of faces
            if (! face->refine( faceRule, tetra->isRear( info._faces[ i ] ) ) ) return false;
          }
          return true;
        }

        static void splitEdge( innertetra_t* tetra, const myrule_t& rule )
        {
          const BisectionInfo& info = instance( rule );

          for( int i=0; i<2; ++i )
          {
            myhface_t* face = tetra->myhface( info._faces[ i ] );

            const face3rule_t faceRule = info._faceRules[i];
         //   std::cout << "Calculated Face  Rule: "<< faceRule << " with TetraRule: " << rule <<  " for " << tetra << " and " << face << std::endl;
            face->refineImmediate ( faceRule );
          }

          // call correct split edge
          info.caller().splitEdge( tetra );
        }

        const CallSplitIF& caller() const { alugrid_assert ( _caller ); return *_caller; }
      };
      // end BisectionInfo

      // return true if further refinement is needed to create conforming closure
      virtual bool markForConformingClosure ()
      {
        alugrid_assert ( myGrid()->conformingClosureNeeded() );
        // check if any of the faces is refined in a bisect fashion
        alugrid_assert( nFaces() == 4 );
        //for the 2d case we only need to check the faces 1-3
        for(int f = 0; f < 4; ++f)
        {
          if( myhface(f) -> down() )
          {
            request(myrule_t::bisect);
            return true;
          }
        }
        // for the real 3d case we also need to check edges
        // if an edge exits, that has children, we also have to refine this tetra
        alugrid_assert ( nEdges() == 6 );
        for (int e=0; e < 6; ++e)
        {
          if( myhedge( e )->down() )
          {
            request ( myrule_t::bisect );
            return true;
          }
        }
        return false;
      }

      // mark edges to prohibit coarsening
      virtual void markEdgeCoarsening ()
      {
        alugrid_assert ( this->myGrid()->conformingClosureNeeded() );
        alugrid_assert ( this->nEdges() == 6 );
        // nothing to do for macro element
        if ( _lvl == 0 ) return;

        // get father
        innertetra_t* father = (innertetra_t * ) this->up();

        for (int e=0; e<6; ++e)
        {
          myhedge_t *edge = father->myhedge( e );
          // the father of a leaf element can only have one non leaf edge
          if ( ! (_req == myrule_t :: crs && edge->down() ) )
          {
            edge->disableEdgeCoarsen();
          }
        }
      }

      void refineImmediate (myrule_t);
      inline void append (innertetra_t * h);

      bool checkTetra( const innertetra_t* tetra, const int  ) const;

      // change coordinates of this element (for ghost elements only)
      void changeVertexCoordinates( const int face, const std::array< std::array<alucoord_t,3>, 8 >& newCoords, const double volume )
      {
        // this should only be called for ghost elements
        alugrid_assert( this->isGhost() );

#ifdef ALUGRIDDEBUG
        for (int i = 0; i < 3; ++i)
        {
          myvertex_t* vx = static_cast< myvertex_t* > (this->myhface(face)->myvertex(i));
          // this test will (and should) fail for vertex projections
          {
            // make sure we got the right face
            alugrid_assert (std::abs(vx->Point()[0]-newCoords[i][0])<1e-8);
            alugrid_assert (std::abs(vx->Point()[1]-newCoords[i][1])<1e-8);
            alugrid_assert (std::abs(vx->Point()[2]-newCoords[i][2])<1e-8);
          }
        }
#endif
        for( int i=0; i<3; ++i )
        {
          //myvertex_t* vx = static_cast< myvertex_t* > (this->myvertex( face, i ));
          myvertex_t* vx = static_cast< myvertex_t* > (this->myhface(face)->myvertex(i));
          vx->setCoordinates( newCoords[ i ] );
        }

        // set vertex opposite to face
        this->myvertex( face )->setCoordinates( newCoords[ 3 ] );

        _volume = volume;
      }

      double computeVolume() const
      {
        // use std::abs to compute volume because in some of the 2d cases this
        // could be negative
        LinearMapping<3,3> map;
        map.buildMapping(myvertex(0)->Point(),myvertex(1)->Point(),myvertex(2)->Point(),myvertex(3)->Point());
        return std::abs(1./6. * map.det(LinearMapping<3,3>::map_t(0)));
      }

      // the element type is obtained from the level of the element
      // under the assumption that on level 0 all elements have type 0
      unsigned char elementType () const { return ((this->macroSimplexTypeFlag() + _lvl) % 3); }

      // sets the new edges for this tetra
      void setNewMapping( innertetra_t*, innertetra_t*, innerface_t* );

    protected:
      void setIndexAndFlag();

    private :
      innertetra_t * _bbb, * _up; // 16 byte
      inner_t * _inner;           //  8 byte
      double _volume;             //  8 byte

      const unsigned char _lvl;  // 1 byte
      signed char _nChild;       // 1 byte
      myrule_t _req, _rule;      // 2 byte   = 8 byte

    private :
      bool checkRule( const myrule_t rule ) const
      {
        // note that children are flipped
        // only edges opposite of the recent vertex introduced are possible
        // the edge of the newface that is in the father is not possible
        static const myrule_t possibleRules0[ 6 ][ 2 ] = {
            { myrule_t :: e02, myrule_t :: e03 }, // possible rules for e01 in child 0
            { myrule_t :: e03, myrule_t :: e01 }, // possible rules for e02 in child 0
            { myrule_t :: e02, myrule_t :: e01 }, // possible rules for e03 in child 0
            { myrule_t :: e01, myrule_t :: e13 }, // possible rules for e12 in child 0
            { myrule_t :: e01, myrule_t :: e12 }, // possible rules for e13 in child 0
            { myrule_t :: e02, myrule_t :: e12 }  // possible rules for e23 in child 0
          };
        static const myrule_t possibleRules1[ 6 ][ 2 ] = {
            { myrule_t :: e13, myrule_t :: e12 }, // possible rules for e01 in child 1
            { myrule_t :: e02, myrule_t :: e23 }, // possible rules for e02 in child 1
            { myrule_t :: e13, myrule_t :: e03 }, // possible rules for e03 in child 1
            { myrule_t :: e23, myrule_t :: e02 }, // possible rules for e12 in child 1
            { myrule_t :: e13, myrule_t :: e03 }, // possible rules for e13 in child 1
            { myrule_t :: e13, myrule_t :: e03 }  // possible rules for e23 in child 1
          };

        if( _up )
        {
          if( _nChild == 0 )
          {
            return ( rule == possibleRules0[ int( _up->_rule )-2 ][ 0 ] ) ||
                   ( rule == possibleRules0[ int( _up->_rule )-2 ][ 1 ] );
          }
          else
          {
            return ( rule == possibleRules1[ int( _up->_rule )-2 ][ 0 ] ) ||
                   ( rule == possibleRules1[ int( _up->_rule )-2 ][ 1 ] );
          }
        }
        else
          return true;
      }

      // Only checks 3d!
      void splitInfo( const myrule_t rule ) const
      {
        /*
        std::cout << std::endl << "Split tetra " << this<< std::endl;
        std::cout << " ( " << this->getIndex() << ", ch" << int( _nChild) << ") with rule " << rule << "  ";
        if( _up )
          std::cout << "father (" << _up->getIndex() << ", ch" << int( _up->_nChild) << ") rule = " << _up->_rule << std::endl;
        std::cout << std::endl;
        const bool chRule = checkRule( rule );
        if( ! chRule )
        {
          std::cout << rule << " not valid " << std::endl;
          //alugrid_assert ( false );
        }*/
      }

      myrule_t suggestRule ( bool longest = false )  const
      {
        // implementation of longest edge refinement
        if(longest)
        {
          myrule_t rule = myrule_t::e01;
          std::multimap<double, myrule_t> rules;
          int i = (this->is2d()) ? 1 : 0;
          int rulenumber = (this->is2d()) ? 5 : 2 ;

          for(; i<4; ++i)
          {
            const alucoord_t (&p0)[ 3 ] = myvertex(i)->Point();
            for(int j=i+1; j < 4; ++j)
            {
              //std::cout << i << j << rulenumber << " "  ;
              const alucoord_t (&p1)[ 3 ] = myvertex(j)->Point();
              double sum = 0;

              for(int k=0; k<3; ++k)
              {
                double diff = p0[k] - p1[k];
                sum += (diff * diff );
              }
              rules.insert(std::make_pair(sum, static_cast<myrule_t>(rulenumber)));
              rulenumber++;
            }
          }
          // if there are multiple edges of largest size, take the one with bigger rule number
          auto rIt = rules.rbegin();
          double maxLength = rIt->first;
          rule = (rules.rbegin())->second;
          rIt++;
          while( std::abs( rIt->first - maxLength ) < 1e-8 )
          {
            if(rIt->second < rule )
              rule = rIt->second;
            rIt++;
          }

          //std::cout << rule <<std::endl;
          alugrid_assert( rule.bisection() );
          return rule;
        }
        //implementation of NVB bisection refinement
        else
        {
          //specialization for the 2d case
          //always thae the edge that is opposite to the most recent introduced vertex
          if( this->is2d() )
          {
            if( _up )
            {
              switch( _up->getrule() ){
                case (myrule_t :: e12):
                  if(this->nChild() == 0)
                    return myrule_t :: e13;
                  if(this->nChild() == 1)
                    return myrule_t :: e23;
                  break;
                case (myrule_t :: e13):
                  if(this->nChild() == 0)
                    return myrule_t :: e12;
                  if(this->nChild() == 1)
                    return myrule_t :: e13;
                  break;
                case (myrule_t :: e23):
                  if(this->nChild() == 0)
                    return myrule_t :: e12;
                  if(this->nChild() == 1)
                    return myrule_t :: e13;
                  break;
                default:
                  alugrid_assert(this->nChild() == 0 || this->nChild() == 1);
                  std::cerr << __FILE__ << __LINE__ << ":  Refinement Edge must not contain Vertex 0" << std::endl;
                  alugrid_assert(false);

              }
            }
            else
            {
              return suggestRule(true);
            }
          }
          //3d NVB specialization
          // This is based on the following property of NVB
          // Once a vertex is part of a refinement edge it is always part of the refinement edge in all descendants that contain it
          // Now we inspect the refinement edge of both father element and grandfather and choose the refinement edge accordingly
          else
          {
            // if  we have a grandfather
            if( _lvl > 1 )
            {
              myrule_t grandFatherRule = _up->_up->getrule();
              myrule_t fatherRule = _up->getrule();
              int fatherChild = _up->nChild();
              int thisChild = this->nChild();
              // vertices of the refinement edge - e.g. e03 -> 0 in child 0 and 3 in child 1
              const int refEdgeVertices [ 6 ][ 2 ] = { { 0, 1}, {0, 2}, {0,3}, {1,2}, {1,3}, {2,3} };
              // local index of new vertex - e.g. e03 -> 3 in child 0 and 2 in child 1
              // for child 0 it is always the other vertex of the refinement edge
              const int newVertices [ 6 ] [ 2 ] = { {1,0}, {2,1}, {3,2}, {2,1}, {3,2}, {3,2} };
              // choose rule based on vertices that are present
              const myrule_t ruleChoicePres  [ 4 ] [ 4 ] = { { -1, myrule_t :: e01, myrule_t :: e02, myrule_t :: e03},
                                                         { myrule_t :: e01, -1, myrule_t :: e12, myrule_t :: e13},
                                                         { myrule_t :: e02, myrule_t :: e12, -1, myrule_t :: e23},
                                                         { myrule_t :: e03, myrule_t :: e13, myrule_t :: e23, -1} };
              // choose rule based on vertices that are not present
              const myrule_t ruleChoice  [ 4 ] [ 4 ] = { { -1, myrule_t :: e23, myrule_t :: e13, myrule_t :: e12},
                                                         { myrule_t :: e23, -1, myrule_t :: e03, myrule_t :: e02},
                                                         { myrule_t :: e13, myrule_t :: e03, -1, myrule_t :: e01},
                                                         { myrule_t :: e12, myrule_t :: e02, myrule_t :: e01, -1} };
              // If elementType == 0 or 2 then the grandfather was of type  0 or 11
              // this is the easier case. Always refine the edge that is still present from the grandfather.
              if( elementType() == 2 || elementType() == 0 )
              {
                int newFromGrandFather = newVertices[int(grandFatherRule) - 2][fatherChild];
                int newFromFather = newVertices[int(fatherRule) - 2][thisChild];
                //We may have to adjust the grandFathervertex, if the child 1 has rotated
                if( thisChild == 1)
                {
                  //e02 maps 1->0
                  if(fatherRule == myrule_t::e02 && newFromGrandFather == 1) newFromGrandFather = 0;
                  //e03 maps 1->0 and 2->1
                  else if (fatherRule == myrule_t::e03 && newFromGrandFather <=2 ) newFromGrandFather -= 1;
                  //e13 maps 2->1
                  else if (fatherRule == myrule_t::e13 && newFromGrandFather ==2 ) newFromGrandFather -= 1;
                }
                //Now we just take the edge, that does not include any of these two vertices
                return ruleChoice[newFromGrandFather][newFromFather];
              }
              //case elementType == 1 (grandfather =2 ) behaves differently if the child is bisectionwise child 1
              //the refinement edge contains the vertex created in the grandfather and
              //the vertex of the father refinement edge, that is contained in the child
              else
              {
                //First we need to find out, whether we have vertices of the refinement edge of the grandfather
                int refEdgeGrandFatherIndex0 = _up->_up->myvertex(refEdgeVertices[int(grandFatherRule) - 2][0])->getIndex();
                int refEdgeGrandFatherIndex1 = _up->_up->myvertex(refEdgeVertices[int(grandFatherRule) - 2][1])->getIndex();
                int newFromGrandFather = newVertices[int(grandFatherRule) - 2][fatherChild];
                int newFromFather = newVertices[int(fatherRule) - 2][thisChild];
                int refEdgeFromFather = refEdgeVertices[int(fatherRule) - 2][thisChild];
                //We may have to adjust the grandFathervertex, if the child 1 has rotated
                if( thisChild == 1)
                {
                  //e02 maps 1->0
                  if(fatherRule == myrule_t::e02 && newFromGrandFather == 1) newFromGrandFather = 0;
                  //e03 maps 1->0 and 2->1
                  else if (fatherRule == myrule_t::e03 && newFromGrandFather <=2 ) newFromGrandFather -= 1;
                  //e13 maps 2->1
                  else if (fatherRule == myrule_t::e13 && newFromGrandFather ==2 ) newFromGrandFather -= 1;
                }
                bool child1 = true;
                for(int i=0; i < 4 ; ++i)
                {
                  if( i == newFromFather || i == newFromGrandFather )
                    continue;
                  if( this->myvertex(i)->getIndex() == refEdgeGrandFatherIndex0 || this->myvertex(i)->getIndex() == refEdgeGrandFatherIndex1 )
                  {
                    child1 = false;
                    break;
                  }
                }

                return child1 ? ruleChoicePres[newFromGrandFather][refEdgeFromFather] : ruleChoice[newFromGrandFather][newFromFather];
              }
            }
            //if we are macro
            //The vertices are ordered in two disjoint sets V0,V1
            //and V1 comes after V0
            //the type is the size of V1 and the refinement edge is
            //the first and last vertex of V0
            //Example: Element: [0,1,2,3]
            //type = 0 -> V1 = [] -> V0 = [0,1,2,3] -> refedge = 0--3
            //type = 2 -> V1 = [2,3] -> V0 = [0,1] -> refedge = 0--1
            else if( _lvl == 0 )
            {
              if(this->elementType() == 0)
                return myrule_t :: e03;
              if(this->elementType() == 1)
                return myrule_t :: e02;
              if(this->elementType() == 2)
                return myrule_t :: e01;
            }
            // if we do not have a grandfather, we know the father rule depending on the type
            else
            {
              alugrid_assert( _lvl == 1 );
              if(this->elementType() == 1)
              {
                if (this->nChild() == 0)
                  return myrule_t :: e02;
                else
                {
                  alugrid_assert (this->nChild() == 1);
                  return myrule_t :: e03;
                }
              }
              if(this->elementType() == 2)
              {
                if (this->nChild() == 0)
                  return myrule_t :: e01;
                else
                {
                  alugrid_assert (this->nChild() == 1);
                  return myrule_t :: e02;
                }
              }
              if(this->elementType() == 0)
              {
                if (this->nChild() == 0)
                  return myrule_t :: e03;
                else
                {
                  alugrid_assert (this->nChild() == 1);
                  return myrule_t :: e13;
                }
              }
            }
          }
          //We should not get here
          std::cerr << __FILE__ << __LINE__ << ":  suggestRule does not suggest a refinement rule" << std::endl;
          alugrid_assert(false);
        }
      }


      inline IndexManagerType & indexManager() {
        return this->myvertex(0)->indexManagerStorage().get( IndexManagerStorageType :: IM_Elements ); }

      double calculateChildVolume(const double) const;

      void split_e01 ();
      void split_e12 ();
      void split_e02 ();
      void split_e23 ();
      void split_e03 ();
      void split_e13 ();

      void splitISO8 ();
      void splitIso4_2d();
    protected :
      myhedge_t * subedge (int,int);
      const myhedge_t * subedge (int,int) const;
      facepair_t subFaces( const int );
      facepair_t subFaces( const int, const myvertex_t*, const myvertex_t* );
      myhface_t * subface (int,int);
      const myhface_t * subface (int i, int j) const;
    public:
      // constructor for refined elements
      TetraTop (int,myhface_t *,bool,myhface_t *,bool,myhface_t *,bool,
                    myhface_t *,bool,innertetra_t *up, int nChild, double vol);
      // constructor for macro elements
      TetraTop (int,myhface_t *,bool,myhface_t *,bool,
                    myhface_t *,bool,myhface_t *,bool, SimplexTypeFlag );
      virtual ~TetraTop ();
      inline innertetra_t * up ();
      inline const innertetra_t * up () const;
      inline innertetra_t * down ();
      inline const innertetra_t * down () const;
      inline innertetra_t * next ();
      inline const innertetra_t * next () const;
      inline innervertex_t * innerVertex ();
      inline const innervertex_t * innerVertex () const;
      inline inneredge_t * innerHedge ();
      inline const inneredge_t * innerHedge () const;
      inline innerface_t * innerHface ();
      inline const innerface_t * innerHface () const;

      inline int level () const;
      inline int nChild () const;
      inline double volume () const;

      SimplexTypeFlag simplexTypeFlag () const { return SimplexTypeFlag(0, this->macroSimplexTypeFlag()); }
    public :
      myrule_t getrule () const;
      myrule_t requestrule () const;
      bool refine ();
      void request (myrule_t);
      using A :: refineBalance;
      bool refineBalance (balrule_t,int);
      bool coarse ();
      bool bndNotifyCoarsen ();

      int  backup (std::ostream &) const;
      void restore (std::istream &);

      int  backup (ObjectStream &) const;
      void restore (ObjectStream &);

      // backup and restore index
      void backupIndex (std::ostream &) const;
      void restoreIndex (std::istream &, RestoreInfo& restoreInfo );

      // backup and restore index
      void backupIndex ( ObjectStream& ) const;
      void restoreIndex (ObjectStream&, RestoreInfo& restoreInfo );
    protected:
      // non-virtual methods of down and innerVertex
      innertetra_t* dwnPtr();
      const innertetra_t* dwnPtr() const;
      inneredge_t* inEd();
      const inneredge_t* inEd() const;
      innerface_t* inFce();
      const innerface_t* inFce() const;

      template <class OutStream_t>
      int  doBackup(OutStream_t &) const;

      template <class InStream_t>
      void doRestore(InStream_t &);

      template< class istream_t >
      void restoreIndexImpl ( istream_t &, RestoreInfo &restoreInfo );
  };

  template < class A > class Periodic3Top : public A {
    public:
      using A :: isRear;
      using A :: myhface;

    protected :
      typedef Periodic3Top < A >          innerperiodic3_t ;
      typedef typename A :: innervertex_t innervertex_t;
      typedef typename A :: inneredge_t   inneredge_t;
      typedef typename A :: innerface_t   innerface_t;
      typedef typename A :: myhedge_t    myhedge_t;
      typedef typename A :: myhface_t    myhface_t;
      typedef typename A :: myrule_t      myrule_t;
      typedef typename A :: balrule_t     balrule_t;
      typedef typename A :: bnd_t         bnd_t;

      void refineImmediate (myrule_t);
      inline void append (innerperiodic3_t * h);

    private :
      innerperiodic3_t * _dwn, * _bbb, * _up;
      // we need two indices since this pointer
      // is available on the two periodic sides
      int _segmentId[ 2 ];
      bnd_t _bt[ 2 ];
      const unsigned char _lvl;
      const signed char _nChild;
      myrule_t _rule;

    private :
      void split_e01 ();
      void split_e12 ();
      void split_e02 ();
      void split_iso4 ();
      void split_bisection ();
    protected :
      myhedge_t * subedge (int,int);
      const myhedge_t * subedge (int,int) const;
      myhface_t * subface (int,int);
      const myhface_t * subface (int i, int j) const;

      // we need this for the boundary segment index
      inline IndexManagerType & indexManager () {
        return  this->myhface(0)->myvertex(0)->indexManagerStorage().get( IndexManagerStorageType :: IM_Bnd );
      }
    public:
      // constructor for macro elements
      inline Periodic3Top (int,myhface_t *,bool,myhface_t *,bool, const bnd_t (&bnd)[2] );
      // construtor for refined elements
      inline Periodic3Top (int,myhface_t *,bool,myhface_t *,bool, innerperiodic3_t * up, int nChild );
      virtual inline ~Periodic3Top ();
      inline innerperiodic3_t * up ();
      inline const innerperiodic3_t * up () const;
      inline innerperiodic3_t * down ();
      inline const innerperiodic3_t * down () const;
      inline innerperiodic3_t * next ();
      inline const innerperiodic3_t * next () const;
      inline innervertex_t * innerVertex ();
      inline const innervertex_t * innerVertex () const;
      inline inneredge_t * innerHedge ();
      inline const inneredge_t * innerHedge () const;
      inline innerface_t * innerHface ();
      inline const innerface_t * innerHface () const;
      inline int level () const;
      inline int nChild () const;
      inline int segmentId (const int) const;
      inline bnd_t bndtype (const int i) const
      {
        alugrid_assert ( i==0 || i==1 );
        return _bt[ i ];
      }
    public :
      myrule_t getrule () const;
      bool refine ();
      void request (myrule_t);
      using A :: refineBalance;
      bool refineBalance (balrule_t,int);
      bool coarse ();
      bool bndNotifyCoarsen ();
    public:
      int  backup (std::ostream &) const;
      void restore (std::istream &);

      int  backup (ObjectStream &) const;
      void restore (ObjectStream &);
    protected:
      template <class OutStream_t>
      int  doBackup(OutStream_t &) const;

      template <class InStream_t>
      void doRestore(InStream_t &);
  };
    //
    //    #    #    #  #          #    #    #  ######
    //    #    ##   #  #          #    ##   #  #
    //    #    # #  #  #          #    # #  #  #####
    //    #    #  # #  #          #    #  # #  #
    //    #    #   ##  #          #    #   ##  #
    //    #    #    #  ######     #    #    #  ######
    //


  // #     #                                  #####  #######
  // #     #  ######    ##     ####   ###### #     #    #      ####   #####
  // #     #  #        #  #   #    #  #            #    #     #    #  #    #
  // #######  #####   #    #  #       #####   #####     #     #    #  #    #
  // #     #  #       ######  #       #            #    #     #    #  #####
  // #     #  #       #    #  #    #  #      #     #    #     #    #  #
  // #     #  #       #    #   ####   ######  #####     #      ####   #


  template < class A > inline typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: dwnPtr () {
    return (_inner) ? _inner->dwn() : 0;
  }

  template < class A > inline const typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: dwnPtr () const {
    return (_inner) ? _inner->dwn() : 0;
  }

  template < class A > inline typename Hface3Top < A > :: inneredge_t * Hface3Top < A > :: inEd () {
    return (_inner) ? _inner->ed() : 0;
  }

  template < class A > inline const typename Hface3Top < A > :: inneredge_t * Hface3Top < A > :: inEd () const {
    return (_inner) ? _inner->ed() : 0;
  }
  template < class A > inline typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: down () {
    return dwnPtr();
  }

  template < class A > inline const typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: down () const {
    return dwnPtr();
  }

  template < class A > inline typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: next () {
    return _bbb;
  }

  template < class A > const typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: next () const {
    return _bbb;
  }

  template < class A > inline int Hface3Top < A > :: level () const {
    return _lvl;
  }

  template < class A > inline int Hface3Top < A > :: nChild () const {
    alugrid_assert ( _nChild >= 0 && _nChild < 4 );
    return _nChild;
  }

  template < class A > inline typename Hface3Top < A > :: innervertex_t * Hface3Top < A > :: innerVertex () {
    return 0;
  }

  template < class A > inline const typename Hface3Top < A > :: innervertex_t * Hface3Top < A > :: innerVertex () const {
    return 0;
  }

  template < class A > inline typename Hface3Top < A > :: inneredge_t * Hface3Top < A > :: innerHedge () {
    return inEd();
  }

  template < class A > inline const typename Hface3Top < A > :: inneredge_t * Hface3Top < A > :: innerHedge () const {
    return inEd();
  }

  template < class A > inline typename Hface3Top < A > :: innervertex_t * Hface3Top < A > :: subvertex (int) {
    alugrid_assert (getrule() == myrule_t :: iso4);
    return 0;
  }

  template < class A > inline const typename Hface3Top < A > :: innervertex_t * Hface3Top < A > :: subvertex (int) const {
    alugrid_assert (getrule() == myrule_t :: iso4);
    return 0;
  }

  template < class A > inline typename Hface3Top < A > :: myhedge_t * Hface3Top < A > :: subedge (int i,int j) {
    alugrid_assert (j == 0 || j == 1);
    return myhedge (i)->subedge (j);
  }

  template < class A > inline const typename Hface3Top < A > :: myhedge_t * Hface3Top < A > :: subedge (int i,int j) const {
    alugrid_assert (j == 0 || j == 1);
    return myhedge (i)->subedge (j);
  }

  template < class A > inline typename Hface3Top < A > :: inneredge_t * Hface3Top < A > :: subedge (int n) {
    inneredge_t * e = inEd();
    for (int i = 0; i < n; ++i ) e = e ? e->next () : 0;
    alugrid_assert (e);
    return e;
  }

  template < class A > inline const typename Hface3Top < A > :: inneredge_t * Hface3Top < A > :: subedge (int n) const {
    const inneredge_t * e = inEd();
    for (int i = 0; i < n; ++i ) e = e ? e->next () : 0;
    alugrid_assert (e);
    return e;
  }

  template < class A > inline typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: subface (int n) {
    innerface_t * f = dwnPtr();
    for (int i = 0; i < n; i++ ) f = f ? f->next () : 0;
    alugrid_assert (f);
    return f;
  }

  template < class A > inline const typename Hface3Top < A > :: innerface_t * Hface3Top < A > :: subface (int n) const {
    const innerface_t * f = dwnPtr();
    for (int i = 0; i < n; i++ ) f = f ? f->next () : 0;
    alugrid_assert (f);
    return f;
  }

  template < class A > inline void Hface3Top < A > :: append (innerface_t * f) {
    alugrid_assert (!_bbb);
    _bbb = f;
    return;
  }

  template < class A > inline typename Hface3Top < A > :: myrule_t Hface3Top < A > :: getrule () const {
    return myrule_t (_rule);
  }

  // constructor called during refinement
  template < class A > inline Hface3Top < A > ::
  Hface3Top (int l, myhedge_t * e0,
    myhedge_t * e1, myhedge_t * e2,
    int nChild ) :
    A (e0, e1, e2),
    _bbb (0), _inner(0) ,
    _lvl (l),
    _nChild (nChild),
    _rule (myrule_t :: nosplit)
  {
    this->setIndexAnd2dFlag( indexManager() );
    if( !( e0->is2d() ) ) this->reset2dFlag();
  #if 0 // NDEBUG
    double n[ 3 ];
    LinearSurfaceMapping( myvertex( 0 )->Point(),
                          myvertex( 1 )->Point(),
                          myvertex( 2 )->Point() ).normal( n );

    if( (n[0]*n[0] + n[1]*n[1] + n[2]*n[2] ) < 1e-8 )
    {
      cout << "Determinant of " << this << " is wrong" <<endl;
      cout << "normal: " << n[0] << "," << n[1] << "," << n[2] << endl;
    }
    alugrid_assert ( (n[0]*n[0] + n[1]*n[1] + n[2]*n[2] ) >= 1e-8 );
  #endif
    alugrid_assert ( checkFace( this, nChild ) );
    return;
  }

  // constructor called while creating macro face
  template < class A > inline Hface3Top < A > ::
  Hface3Top (int l, myhedge_t * e0,
    myhedge_t * e1, myhedge_t * e2) :
    A (e0, e1, e2),
    _bbb (0), _inner (0),
    _lvl (l),
    _nChild (0),
    _rule (myrule_t :: nosplit)
  {
    this->setIndexAnd2dFlag( indexManager() );
    if( !( e0->is2d() ) ) this->reset2dFlag();
    alugrid_assert ( checkFace( this, nChild() ) );
  }

  template < class A > inline Hface3Top < A > :: ~Hface3Top ()
  {
    this->freeIndex( indexManager() );
    if (_bbb) delete _bbb;
    if (_inner) delete _inner;
    return;
  }

  // #     #                          #####  #######
  // #     #  #####   #    #  #####  #     #    #      ####   #####
  // #     #  #    #  ##   #  #    #       #    #     #    #  #    #
  // #######  #####   # #  #  #    #  #####     #     #    #  #    #
  // #     #  #    #  #  # #  #    #       #    #     #    #  #####
  // #     #  #    #  #   ##  #    # #     #    #     #    #  #
  // #     #  #####   #    #  #####   #####     #      ####   #

  // serial macro bnd constructor
  template < class A > inline Hbnd3Top < A > ::
  Hbnd3Top (int l, myhface_t * f, int i, const bnd_t bt) :
    A (f, i ),
    _bbb (0), _dwn (0), _up (0) ,
    _bt( bt )
  {
    // set level (declared in hbdnseg3_GEO to save memory usage)
    _lvl = l;

    // set index of boundary segment
    this->setIndex( indexManager().getIndex() );

    // for macro bnd faces store current index as segment index
    _segmentId = this->getIndex();

    // set boundary id
    setBoundaryId( _bt );
    return;
  }

  template < class A > inline Hbnd3Top < A > ::
  Hbnd3Top (int l, myhface_t * f, int i,
            innerbndseg_t * up, bnd_t bt,
            Gitter::helement_STI * gh, int gFace ) :
    A (f, i ), _bbb (0), _dwn (0), _up (up) ,
    _bt (bt)
  {
    // set level (declared in hbdnseg3_GEO to save memory usage)
    _lvl = l;

    alugrid_assert ( this->myGrid()->ghostCellsEnabled() && _bt == A::closure ? gh != 0 : true );
    // store ghost element
    typedef Gitter :: ghostpair_STI ghostpair_STI;
    this->setGhost ( ghostpair_STI (gh , gFace) );

    // set index of boundary segment
    this->setIndex( indexManager().getIndex() );

    if( _up )
    {
      // get segment index from father if existent
      _segmentId = _up->_segmentId;
      // set boundary projection from father
      this->_pvPtr = _up->_pvPtr;
    }
    else
      _segmentId = this->getIndex();

    setBoundaryId( _bt );

    return;
  }

  template < class A > inline Hbnd3Top < A > :: ~Hbnd3Top ()
  {
    // free index
    indexManager().freeIndex( this->getIndex() );

    // detach leaf entities
    if (this->isLeafEntity()) this->detachleafs();
    // delete down and next
    if (_bbb) delete _bbb;
    if (_dwn) delete _dwn;
    return;
  }

  template< class A > inline void Hbnd3Top < A >::
  setBoundaryId (const int id )
  {
    // set my id to the same as bnd
    this->setBndId( id );
    myhface_t & face = *(myhface(0));
    face.setBndId( id );
    // 3 vertices and edges
    for(int i=0; i<3; ++i)
    {
      face.myvertex(i)->setBndId( id );
      face.myhedge(i)->setBndId( id );
    }
  }

  template < class A > inline int Hbnd3Top < A > :: segmentId () const {
    return _segmentId;
  }

  template < class A > inline int Hbnd3Top < A > :: level () const {
    return _lvl;
  }

  template < class A > inline typename Hbnd3Top < A > :: innerbndseg_t * Hbnd3Top < A > :: next () {
    return _bbb;
  }

  template < class A > inline const typename Hbnd3Top < A > :: innerbndseg_t * Hbnd3Top < A > :: next () const {
    return _bbb;
  }

  template < class A > inline typename Hbnd3Top < A > :: innerbndseg_t * Hbnd3Top < A > :: down () {
    return _dwn;
  }

  template < class A > inline const typename Hbnd3Top < A > :: innerbndseg_t * Hbnd3Top < A > :: down () const {
    return _dwn;
  }

  template < class A > inline typename Hbnd3Top < A > :: innerbndseg_t * Hbnd3Top < A > :: up () {
    return _up;
  }

  template < class A > inline const typename Hbnd3Top < A > :: innerbndseg_t * Hbnd3Top < A > :: up () const {
    return _up;
  }

  template < class A > inline void Hbnd3Top < A > :: append (innerbndseg_t * b) {
    alugrid_assert (_bbb == 0);
    _bbb = b;
    return;
  }

  // #######                                 #######
  //    #     ######   #####  #####     ##      #      ####   #####
  //    #     #          #    #    #   #  #     #     #    #  #    #
  //    #     #####      #    #    #  #    #    #     #    #  #    #
  //    #     #          #    #####   ######    #     #    #  #####
  //    #     #          #    #   #   #    #    #     #    #  #
  //    #     ######     #    #    #  #    #    #      ####   #

  template < class A > inline typename TetraTop < A > :: innertetra_t * TetraTop < A > :: dwnPtr() {
    return (_inner) ? _inner->dwn() : 0;
  }

  template < class A > inline const typename TetraTop < A > :: innertetra_t * TetraTop < A > :: dwnPtr() const {
    return (_inner) ? _inner->dwn() : 0;
  }

  template < class A > inline typename TetraTop < A > :: inneredge_t * TetraTop < A > :: inEd() {
    return (_inner) ? _inner->ed() : 0;
  }

  template < class A > inline const typename TetraTop < A > :: inneredge_t * TetraTop < A > :: inEd() const {
    return (_inner) ? _inner->ed() : 0;
  }

  template < class A > inline typename TetraTop < A > :: innerface_t * TetraTop < A > :: inFce() {
    return (_inner) ? _inner->fce() : 0;
  }

  template < class A > inline const typename TetraTop < A > :: innerface_t * TetraTop < A > :: inFce() const {
    return (_inner) ? _inner->fce() : 0;
  }

  template < class A > inline double TetraTop < A > :: calculateChildVolume (const double childVolume) const
  {
    // if vertex projection is available on a neighbor
    // volume has to be recalculated
    //return ( this->myGrid()->vertexProjection() ) ? -1.0 : childVolume;
    return -1.;
  }

  template < class A > inline int TetraTop < A > :: level () const {
    return _lvl;
  }

  template < class A > inline double TetraTop < A > :: volume () const {
    return _volume;
  }

  template < class A > inline int TetraTop < A > :: nChild () const {
    alugrid_assert ( _nChild >= 0 && _nChild < 8 );
    return _nChild;
  }

  template < class A > inline typename TetraTop < A > :: innertetra_t * TetraTop < A > :: up () {
    return _up;
  }
  template < class A > inline const typename TetraTop < A > :: innertetra_t * TetraTop < A> :: up () const {
    return _up;
  }

  template < class A > inline typename TetraTop < A > :: innertetra_t * TetraTop < A > :: down () {
    return dwnPtr();
  }

  template < class A > inline const typename TetraTop < A > :: innertetra_t * TetraTop < A > :: down () const {
    return dwnPtr();
  }

  template < class A > inline typename TetraTop < A > :: innertetra_t * TetraTop < A > :: next () {
    return _bbb;
  }

  template < class A > inline const typename TetraTop < A > :: innertetra_t * TetraTop < A > :: next () const {
    return _bbb;
  }

  template < class A > inline typename TetraTop < A > :: innervertex_t * TetraTop < A > :: innerVertex () {
    return 0;
  }

  template < class A > inline const typename TetraTop < A > :: innervertex_t * TetraTop < A > :: innerVertex () const {
    return 0;
  }

  template < class A > inline typename TetraTop < A > :: inneredge_t * TetraTop < A > :: innerHedge () {
    return inEd();
  }

  template < class A > inline const typename TetraTop < A > :: inneredge_t * TetraTop < A > :: innerHedge () const {
    return inEd();
  }

  template < class A > inline typename TetraTop < A > :: innerface_t * TetraTop < A > :: innerHface () {
    return inFce();
  }

  template < class A > inline const typename TetraTop < A > :: innerface_t * TetraTop < A > :: innerHface () const {
    return inFce();
  }

  template < class A > inline void TetraTop < A > :: append (TetraTop < A > * h) {
    alugrid_assert (_bbb == 0);
    _bbb = h;
    return;
  }

  template < class A > inline typename TetraTop < A > :: myrule_t TetraTop < A > :: getrule () const {
    return myrule_t (_rule);
  }

  template < class A > inline typename TetraTop < A > :: myrule_t TetraTop < A > :: requestrule () const {
    return myrule_t (_req);
  }

  // --request
  template < class A > inline void TetraTop < A > :: request (myrule_t r)
  {
    alugrid_assert (r.isValid ());

    if( r == myrule_t :: bisect )
    {
      // this can only be used when conforming closure is enabled
      alugrid_assert ( this->myGrid()->conformingClosureNeeded() );

      // we always split edge 0 and 3 following
      // the idea of Stevenson, Nochetto, Veser, Siebert
      // suggestRule returns the correct splitting edge
      // according to ALUGrid's rules
      _req = suggestRule();
    }
    else
    {
      // all other cases
      _req = r;
    }
    return;
  }

  template < class A > inline bool TetraTop < A > :: bndNotifyCoarsen () {
    return true;
  }

  // ######                                                           #####  #######
  // #     #  ######  #####      #     ####   #####      #     ####  #     #   #      ####   #####
  // #     #  #       #    #     #    #    #  #    #     #    #    #       #   #     #    #  #    #
  // ######   #####   #    #     #    #    #  #    #     #    #       #####    #     #    #  #    #
  // #        #       #####      #    #    #  #    #     #    #            #   #     #    #  #####
  // #        #       #   #      #    #    #  #    #     #    #    # #     #   #     #    #  #
  // #        ######  #    #     #     ####   #####      #     ####   #####    #      ####   #

  template < class A > inline Periodic3Top < A > ::
  Periodic3Top (int l, myhface_t * f0, bool t0,
    myhface_t * f1, bool t1, const bnd_t (&bt)[2] )
   : A (f0, t0, f1, t1)
   , _dwn (0), _bbb (0), _up(0)
   , _lvl (l)
   , _nChild(0)
   , _rule (myrule_t :: nosplit)
  {
    IndexManagerType& im = indexManager();
    // get index
    this->setIndex( im.getIndex() );

    // take macro index as segment index
    _segmentId[ 0 ] = this->getIndex();
    _segmentId[ 1 ] = im.getIndex();

    // store boundary ids
    _bt[ 0 ] = bt[ 0 ];
    _bt[ 1 ] = bt[ 1 ];
  }

  template < class A > inline Periodic3Top < A > ::
  Periodic3Top (int l, myhface_t * f0, bool t0, myhface_t * f1, bool t1, innerperiodic3_t * up, int nChild )
    : A (f0, t0, f1, t1)
    , _dwn (0), _bbb (0), _up(up)
    , _lvl (l)
    , _nChild (nChild)
    , _rule (myrule_t :: nosplit)
  {
    // get index
    this->setIndex( indexManager().getIndex() );

    // get segment index from father
    alugrid_assert ( _up );
    _segmentId[ 0 ] = _up->_segmentId[ 0 ];
    _segmentId[ 1 ] = _up->_segmentId[ 1 ];

    // store boundary ids
    _bt[ 0 ] = _up->_bt[ 0 ];
    _bt[ 1 ] = _up->_bt[ 1 ];
  }

  template < class A > inline Periodic3Top < A > :: ~Periodic3Top ()
  {
    // free index
    IndexManagerType& im = indexManager();
    // free indices
    im.freeIndex( this->getIndex() );
    // only on macro boundary free segment index
    if( level() == 0 ) im.freeIndex( _segmentId[ 1 ] );

    // delete down and next
    if (_bbb) delete _bbb;
    if (_dwn) delete _dwn;
  }

  template < class A > inline int Periodic3Top < A > :: level () const {
    return _lvl;
  }

  template < class A > inline int Periodic3Top < A > :: segmentId (const int fce) const {
    alugrid_assert ( fce == 0  || fce == 1 );
    return _segmentId[ fce ];
  }

  template < class A > inline int Periodic3Top < A > :: nChild () const {
    alugrid_assert ( _nChild >= 0 && _nChild < 4 );
    return _nChild;
  }

  template < class A > inline typename Periodic3Top < A > :: innerperiodic3_t * Periodic3Top < A > :: up () {
    return _up;
  }
  template < class A > inline const typename Periodic3Top < A > :: innerperiodic3_t * Periodic3Top < A> :: up () const {
    return _up;
  }

  template < class A > inline typename Periodic3Top < A > :: innerperiodic3_t * Periodic3Top < A > :: down () {
    return _dwn;
  }

  template < class A > inline const typename Periodic3Top < A > :: innerperiodic3_t * Periodic3Top < A > :: down () const {
    return _dwn;
  }

  template < class A > inline typename Periodic3Top < A > :: innerperiodic3_t * Periodic3Top < A > :: next () {
    return _bbb;
  }

  template < class A > inline const typename Periodic3Top < A > :: innerperiodic3_t * Periodic3Top < A > :: next () const {
    return _bbb;
  }

  template < class A > inline typename Periodic3Top < A > :: innervertex_t * Periodic3Top < A > :: innerVertex () {
    return 0;
  }

  template < class A > inline const typename Periodic3Top < A > :: innervertex_t * Periodic3Top < A > :: innerVertex () const {
    return 0;
  }

  template < class A > inline typename Periodic3Top < A > :: inneredge_t * Periodic3Top < A > :: innerHedge () {
    return 0;
  }

  template < class A > inline const typename Periodic3Top < A > :: inneredge_t * Periodic3Top < A > :: innerHedge () const {
    return 0;
  }

  template < class A > inline typename Periodic3Top < A > :: innerface_t * Periodic3Top < A > :: innerHface () {
    return 0;
  }

  template < class A > inline const typename Periodic3Top < A > :: innerface_t * Periodic3Top < A > :: innerHface () const {
    return 0;
  }

  template< class A >
  inline void Periodic3Top< A >::append ( Periodic3Top< A > * h )
  {
    alugrid_assert (_bbb == 0);
    _bbb = h;
    return;
  }

  template< class A >
  inline typename Periodic3Top< A >::myrule_t Periodic3Top < A > :: getrule () const
  {
    return myrule_t( _rule );
  }

  template< class A >
  inline void Periodic3Top< A >::request ( myrule_t )
  {
    // Einen Request zur Verfeinerung zu setzen, ist vorl"aufig inhaltlich nicht
    // vorgesehen und wird deshalb ignoriert (leise).
  }

} // namespace ALUGrid

#endif // #ifndef GITTER_TETRATOP_H_INCLUDED
