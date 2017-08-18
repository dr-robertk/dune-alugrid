/** include config file generated by configure
 *  (i.e., know what grids are present, etc)
 *  this should always be included first */
#include <config.h>
/** standard headers **/
#include <iostream>

#include <dune/common/version.hh>
/** dune (mpi, field-vector and grid type for dgf) **/
#include <dune/common/fvector.hh>

#include <dune/grid/io/file/vtk/vtkwriter.hh>

/** backup and restore facility */
#include <dune/grid/common/backuprestore.hh>

// include mpi stuff before sionlib header
#include <dune/common/parallel/mpihelper.hh>

// parallel dgf reading
#include "paralleldgf.hh"

// SIONlib backup/restore
#include "sionlib.hh"

void restoreGrid( const std::string& method,
                  std::istream& in )
{
  typedef Dune::GridSelector::GridType Grid;
  typedef Grid::Partition< Dune::Interior_Partition >::LeafGridView GridView;

  /* Grid construction using restore method */
  // the restore method returns an object of type Grid*
  // the user takes control of this object
  Grid* gridPtr = Dune::BackupRestoreFacility< Grid > :: restore( in );

  // get grid reference
  Grid &grid = *gridPtr;

  /* get view to leaf grid */
  GridView gridView = grid.leafGridView< Dune::Interior_Partition >();

  /* create VTK writer */
  Dune::VTKWriter< GridView > vtk( gridView );

  /* output the restored grid */
  vtk.write( method );

  // delete grid
  delete gridPtr ;
}

// method
// ------
void method ( const std::string& gridFileName,
              const int startLvl, const int rank )
{
  typedef Dune::GridSelector::GridType Grid;

  const int startLevel = startLvl * Dune :: DGFGridInfo< Grid > :: refineStepsForHalf();

  typedef Grid::Partition< Dune::Interior_Partition >::LeafGridView GridView;

  // backup and restore stream
  std::stringstream stream ;
  std::string filename;

  {
    /* Grid construction ... */
    Dune::GridPtr< Grid > gridPtr( gridFileName );

    Grid &grid = *gridPtr;

    // call load balance to partition the grid
    grid.loadBalance();

    // verbosity on rank 0
    const bool verboseRank = grid.comm().rank() == 0 ;

    /* ... some global refinement steps */
    if( verboseRank )
      std::cout << "globalRefine: " << startLevel << std::endl;
    grid.globalRefine( startLevel );

    /* get view to leaf grid */
    GridView gridView = grid.leafGridView< Dune::Interior_Partition >();

    /* create VTK writer */
    Dune::VTKWriter< GridView > vtk( gridView );

    /* output the restored grid */
    vtk.write( "original" );

    // instead of stringstream we could also use any other std::ostream
    Dune::BackupRestoreFacility< Grid > :: backup( grid, stream );

    // also write backup to hard disk
    std::stringstream filenamestr;
    filenamestr << "checkpoint." << grid.comm().rank();
    // store file name for restore
    filename = filenamestr.str();

    // create checkpoint file
    std::ofstream file( filename );
    // if file was opened
    if( file )
    {
      // write backup to given file
      Dune::BackupRestoreFacility< Grid > :: backup( grid, file );
    }

    // write backup to hard drive using SIONlib
    // (alternative to writing one file per core above)
    backupSION( "sioncheckpoint", rank, stream );
  } // here GridPtr is destroyed

  ///////////////////////////////////////////////////////////////
  //
  // Now we will recreate the grid using the different backups
  //
  ///////////////////////////////////////////////////////////////

  // restore grid from above stringstream
  {
    restoreGrid( "internalrestore", stream );
  }

  // restore grid from stream sionlib file
  {
    std::stringstream restore ;
    // write backup to hard drive using SIONlib
    restoreSION( "sioncheckpoint", rank, restore );

    restoreGrid( "sionrestore", restore );
  }

  // restore grid from file stream
  std::ifstream file( filename );
  if( file )
  {
    restoreGrid( "filerestore", file );
  }
}

/***************************************************
 ** main program with parameters:                 **
 ** 1) number of problem to use (initial data...) **
 ** 2) number of global refinement steps          **
 ** 3) maximal level to use during refinement     **
 ***************************************************/
int main ( int argc , char **argv )
try
{
  /* initialize MPI, finalize is done automatically on exit */
  Dune::MPIHelper &mpi = Dune::MPIHelper::instance( argc, argv );

  if( argc < 1 )
  {
    /* display usage */
    if( mpi.rank() == 0 )
      std::cout << "Usage: " << argv[ 0 ] << " [dgf file] [startLevel (optional)]" << std::endl;
    return 0;
  }

#if HAVE_ALUGRID
  if( mpi.rank() == 0 )
    std::cout << "WARNING: Using old ALUGrid version from dune-grid." << std::endl;
#endif

  /* create problem */
  const char* gridFileName = (argc > 1) ? argv[ 1 ] : "./dgf/unitcube3d.dgf";

  /* get level to use for computation */
  const int startLevel = (argc > 2 ? atoi( argv[ 2 ] ) : 0);

  // method does a backup and restore of the given grid
  method( gridFileName, startLevel, mpi.rank() );

  /* done */
  return 0;
}
catch( const std::exception &e )
{
  std::cout << "STL ERROR: " << e.what() << std::endl;
  return 1;
}
#if ! DUNE_VERSION_NEWER(DUNE_COMMON,2,5)
catch( const Dune::Exception &e )
{
  std::cout << "DUNE ERROR: " << e << std::endl;
  return 1;
}
#endif
#if ! HAVE_ALUGRID
catch( const ALUGrid::ALUGridException& e )
{
  std::cout << "ALUGrid ERROR: " << e.what() << std::endl;
  return 1;
}
#endif
catch( ... )
{
  std::cout << "Unknown ERROR" << std::endl;
  return 1;
}
