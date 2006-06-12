// (c) Robert Kloefkorn 2004 -- 2005 
#ifndef GITTER_DUNE_PLL_IMPL_H_INCLUDED
#define GITTER_DUNE_PLL_IMPL_H_INCLUDED

#ifdef _ANSI_HEADER
  using namespace std;
  #include <numeric>
#else
#endif

#include "gitter_dune_impl.h"
 
#include "gitter_pll_impl.h"
#include "gitter_pll_ldb.h"
  
class GitterDunePll : public GitterBasisPll , public virtual GitterDuneBasis
{
 
  virtual IteratorSTI < Gitter :: helement_STI > * 
    leafIterator (const Gitter :: helement_STI *) ;
  virtual IteratorSTI < Gitter :: helement_STI > * 
    leafIterator (const IteratorSTI < Gitter :: helement_STI > *) ;
  
protected:  
  bool balanceGrid_;

public:
  typedef Gitter :: Geometric Geometric;
  typedef GitterDuneImpl :: Objects  Objects;
  
  GitterDunePll (const char * filename , MpAccessLocal &mp) 
    : GitterBasisPll (filename,mp) , balanceGrid_ (false) 
  {
#ifndef NDEBUG
    __STATIC_myrank = mp.myrank(); 
#endif
    /*
    // logfile is defined in gitter_impl.h    
    char logFileName [32];
    sprintf(logFileName,"logfile.%d",mpAccess().myrank());
    cerr << "open logfile = " << logFileName << "\n";

    logFile.clear();
    logFile.open ( logFileName );
    logFile << "logfile of processor " << mpAccess().myrank() << "\n";
    logFile.flush();
    */
  };

  ~GitterDunePll () {
    //logFile.close();
  }

  bool refine (); 

  void coarse ();

  // adapts and calls notify and loadBalancer  
  bool dAdapt ();

  // adapts and calls notify and loadBalancer, also preCoarsening and
  // postRefinement is called 
  bool duneAdapt (AdaptRestrictProlongType & arp);

  // return true if grid has to be balanced again 
  bool duneNotifyNewGrid ();

  bool duneLoadBalance () ; // call loadBalancer 
  bool duneLoadBalance (GatherScatterType & , AdaptRestrictProlongType & arp ) ; // call loadBalancer a

  void duneRepartitionMacroGrid (LoadBalancer :: DataBase &, GatherScatterType & gs) ;
  void repartitionMacroGrid (LoadBalancer :: DataBase &) ;
 
  // notifyGridChanges for dune
  void duneNotifyGridChanges (); 
  
  // exchange changed elements  
  void duneExchangeDynamicState ();
  
  // exchange data of dune 
  void duneExchangeData (GatherScatterType &, bool leaf = false );

  // communication of data 
  void ALUcomm (
         GatherScatterType & vertexData ,
         GatherScatterType & edgeData,
         GatherScatterType & faceData ,
	 GatherScatterType & elementData );

  // return indexmanger 
  IndexManagerType & indexManager(int codim)
  {
    return containerPll().indexManager(codim);
  }

private:
  enum { transmittedData = 1 , noData = 0 };

  template <class ObjectStreamType, class HItemType>
  void sendSlaves (ObjectStreamType & sendBuff,
      HItemType * determType,
      GatherScatterType & dataHandle , const int link ) ; 
    
  template <class ObjectStreamType, class HItemType>
  void unpackOnMaster(ObjectStreamType & recvBuff,
      HItemType * determType,
      GatherScatterType & dataHandle , 
      const int nl, const int link) ; 
    
  template <class ObjectStreamType, class HItemType>
  void sendMaster(ObjectStreamType & sendBuff,
      HItemType * determType,
      GatherScatterType & dataHandle , 
      const int nl, const int myLink) ; 
    
  template <class ObjectStreamType, class HItemType>
  void unpackOnSlaves(ObjectStreamType & recvBuff,
      HItemType * determType,
      GatherScatterType & dataHandle , 
      const int nOtherLinks, const int myLink) ; 

  // only echange leaf data 
  //void duneExchangeDataLeaf (GatherScatterType &);

  // exchange all data 
  void duneExchangeDataAll (GatherScatterType &);

public:
  typedef GitterPll :: vertex_STI vertex_STI; 
  typedef GitterPll :: hedge_STI  hedge_STI; 
  typedef GitterPll :: hface_STI  hface_STI; 

  pair < IteratorSTI < vertex_STI > *, IteratorSTI < vertex_STI > *> borderIteratorTT (const vertex_STI *, int) ;
  pair < IteratorSTI < hedge_STI  > *, IteratorSTI < hedge_STI  > *> borderIteratorTT (const hedge_STI  *, int) ;
  pair < IteratorSTI < hface_STI  > *, IteratorSTI < hface_STI  > *> borderTteratorTT (const hface_STI  *, int) ;
    
};
#endif
