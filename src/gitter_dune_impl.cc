#ifndef GITTER_DUNE_IMPL_CC_INCLUDED
#define GITTER_DUNE_IMPL_CC_INCLUDED

#include "gitter_dune_impl.h"

void GitterDuneBasis :: backupIndices (ostream & out)
{
  // backup indices 
  bool indices = true; out.put(indices);  // indices == true

  // store max indices 
  for(int i=0; i< numOfIndexManager ; i++)
    indexManager(i).backupIndexSet(out);

  { // backup index of elements 
    AccessIterator <helement_STI> :: Handle ew (container ()) ;
    for (ew.first () ; ! ew.done () ; ew.next ()) ew.item ().backupIndex (out) ;
  }
  {
    // backup index of vertices 
    LeafIterator < vertex_STI > w ( *this );
    for( w->first(); ! w->done() ; w->next () ) w->item().backupIndex(out);
  }

  return ;
}

// go down all children and check index 
inline void GitterDuneBasis :: 
goDownHelement( Gitter::helement_STI & el , vector<bool> & idxcheck)
{
  typedef Gitter :: helement_STI ElType;
  assert( el.getIndex() < idxcheck.size() );
  idxcheck[ el.getIndex() ] = false;
  for( ElType * ch = el.down() ; ch ; ch = ch->next())
    goDownHelement( *ch , idxcheck );

  return ;
}

void GitterDuneBasis ::restoreIndices (istream & in) 
{
  bool indices = in.get();
  if(indices)
  {
    for(int i=0; i< numOfIndexManager ; i++)
      this->indexManager(i).restoreIndexSet( in );

    // restore index of elements 
    {
      AccessIterator < helement_STI >:: Handle ew(container());
      for ( ew.first(); !ew.done(); ew.next()) ew.item().restoreIndex (in);
    }
    // restore index of vertices
    {
      LeafIterator < vertex_STI > w ( *this );
      for( w->first(); ! w->done() ; w->next () ) w->item().restoreIndex(in);
    }

    { // reconstruct holes 
      {
        enum { elements = 0 };
        // for elements 
        int idxsize = this->indexManager(elements).getMaxIndex();
        vector < bool > checkidx ( idxsize );
        for(int i=0; i<idxsize; i++) checkidx[i] = true;

        AccessIterator < helement_STI >:: Handle ew(container());
        for ( ew.first(); !ew.done(); ew.next())
        {
          goDownHelement( ew.item() , checkidx );
        }

        for(int i=0; i<idxsize; i++)
        {
          if(checkidx[i] == true)
            this->indexManager(elements).freeIndex(i);
        }
      }
      {
        enum { vertices = 3 };
        // for vertices 
        LeafIterator < vertex_STI > w ( *this );
        int idxsize = this->indexManager(vertices).getMaxIndex();

        vector < bool > checkidx ( idxsize );
        for(int i=0; i<idxsize; i++) checkidx[i] = true;
        for( w->first(); ! w->done() ; w->next () )
        {
          assert( w->item().vertexIndex() < checkidx.size() );
          checkidx[ w->item().vertexIndex() ] = false;
        }

        for(int i=0; i<idxsize; i++)
        {
          if(checkidx[i] == true)
            this->indexManager(vertices).freeIndex(i);
        }
      }
    }
  }
  else 
  {
    cerr<< "WARNING: indices not read! file = "<< __FILE__ << ", line = " << __LINE__ << "\n";
  }
    
  return ;
}

// wird von Dune verwendet 
void GitterDuneBasis :: duneBackup (const char * fileName)
{
  // diese Methode wird von der Dune Schnittstelle aufgerufen und ruft
  // intern lediglich backup (siehe oben) und backupCMode des Makrogitters
  // auf, allerdings wird hier der path und filename in einer variablen
  // uebergeben 

  assert (debugOption (20) ? (cout << "**INFO GitterDuneBasis :: duneBackup (const char * = \""
                       << fileName << "\") " << endl, 1) : 1) ;

  ofstream out (fileName) ;
  if (!out) {
    cerr << "**WARNUNG (IGNORIERT) GitterDuneBasis :: duneBackup (const char *, double) Fehler beim Anlegen von < "
         << (fileName ? fileName : "null") << " >" << endl ;
  }
  else
  {
    FSLock lock (fileName) ;
    Gitter :: backup (out) ;
    GitterDuneBasis :: backupIndices (out) ;

    {
      char *fullName = new char[strlen(fileName)+20];
      if(!fullName)
      {
        cerr << "**WARNUNG GitterDuneBasis :: duneBackup (, const char *, double) :";
        cerr << "couldn't allocate fullName! " << endl;
        abort();
      }
      sprintf(fullName,"%s.macro",fileName);
      ofstream macro (fullName) ;

      if(!macro)
      {
        cerr << "**WARNUNG (IGNORIERT) GitterDuneBasis :: duneBackup (const char *, const char *) Fehler beim Anlegen von < "
         << (fullName ? fullName : "null") << " >" << endl ;
      }
      else
      {
        container ().backupCMode (macro) ;
      }
      delete [] fullName;
    }
  }
  return ;
}

// wird von Dune verwendet 
void GitterDuneBasis :: duneRestore (const char * fileName)
{
  // diese Methode wird von der Dune Schnittstelle aufgerufen 
  // diese Methode ruft intern restore auf, hier wird lediglich 
  // der path und filename in einer variablen uebergeben

  assert (debugOption (20) ? (cout << "**INFO GitterDuneBasis :: duneRestore (const char * = \""
                 << fileName << "\") " << endl, 1) : 1) ;

  ifstream in (fileName) ;
  if (!in) {
    cerr << "**WARNUNG (IGNORIERT) GitterDuneBasis :: duneRestore (const char *, double & ) Fehler beim \"Offnen von < "
         << (fileName ? fileName : "null") << " > " << endl ;
  } else {
    Gitter :: restore (in) ;
    GitterDuneBasis :: restoreIndices (in);
  }
  return ;
}


#endif
