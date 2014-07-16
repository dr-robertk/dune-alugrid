#ifndef DUNE_ALU_PERSISTENTCONTAINER_HH
#define DUNE_ALU_PERSISTENTCONTAINER_HH

#include <dune/grid/utility/persistentcontainer.hh>
#include <dune/grid/utility/persistentcontainervector.hh>

#include <dune/alugrid/grid.hh>

namespace Dune
{

  // ALUGridPersistentContainer
  // --------------------------

  template< class G, class T >
  class ALUGridPersistentContainer
  : public PersistentContainerVector< G, typename G::HierarchicIndexSet, std::vector< T > >
  {
    typedef PersistentContainerVector< G, typename G::HierarchicIndexSet, std::vector< T > > Base;

  public:
    typedef typename Base::Grid Grid;
    typedef typename Base::Value Value;

    /** \brief \deprecated typedef of class Grid */
    typedef typename Base::Grid GridType;

    ALUGridPersistentContainer ( const Grid &grid, int codim, const Value &value = Value() )
    : Base( grid.hierarchicIndexSet(), codim, value )
    {}
  };


  // PersistentContainer for ALUGrid
  // -------------------------------

  template< int dim, int dimworld, ALUGridElementType eltype, ALUGridRefinementType refinementtype, class Comm, class T >
  class PersistentContainer< ALUGrid< dim, dimworld, eltype, refinementtype, Comm >, T >
  : public ALUGridPersistentContainer< ALUGrid< dim, dimworld, eltype, refinementtype, Comm >, T >
  {
    typedef ALUGridPersistentContainer< ALUGrid< dim, dimworld, eltype, refinementtype, Comm >, T > Base;

  public:
    typedef typename Base::Grid Grid;
    typedef typename Base::Value Value;

    /** \brief \deprecated typedef of class Grid */
    typedef typename Base::Grid GridType;

    PersistentContainer ( const Grid &grid, int codim, const Value &value = Value() )
    : Base( grid, codim, value )
    {}
  };

#if 0
  template< int dim, int dimworld, ALU2DSPACE ElementType elType, class T >
  class PersistentContainer< ALU2dGrid< dim, dimworld, elType >, T >
  : public ALUGridPersistentContainer< ALU2dGrid< dim, dimworld, elType >, T >
  {
    typedef ALUGridPersistentContainer< ALU2dGrid< dim, dimworld, elType >, T > Base;

  public:
    typedef typename Base::Grid Grid;
    typedef typename Base::Value Value;

    /** \brief \deprecated typedef of class Grid */
    typedef typename Base::Grid GridType;

    PersistentContainer ( const Grid &grid, int codim, const Value &value = Value() )
    : Base( grid, codim, value )
    {}
  };
#endif  //if 0

  template< int actualDim, int actualDimw, ALU3dGridElementType elType, class Comm, class T >
  class PersistentContainer< ALU3dGrid< actualDim, actualDimw, elType, Comm >, T >
  : public ALUGridPersistentContainer< ALU3dGrid< actualDim, actualDimw, elType, Comm >, T >
  {
    typedef ALUGridPersistentContainer< ALU3dGrid< actualDim, actualDimw, elType, Comm >, T > Base;

  public:
    typedef typename Base::Grid Grid;
    typedef typename Base::Value Value;

    /** \brief \deprecated typedef of class Grid */
    typedef typename Base::Grid GridType;

    PersistentContainer ( const Grid &grid, int codim, const Value &value = Value() )
    : Base( grid, codim, value )
    {}
  };

} // namespace Dune

#endif // #ifndef DUNE_ALU_PERSISTENTCONTAINER_HH
