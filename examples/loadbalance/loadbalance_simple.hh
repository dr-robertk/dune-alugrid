#ifndef LOADBALANCE_SIMPLE_HH
#define LOADBALANCE_SIMPLE_HH

/********************************************************************
 *
 *  Simple repartition handle for ALUGrid
 *
 ********************************************************************/
template< class Grid >
struct SimpleLoadBalanceHandle
{
  typedef typename Grid :: Traits :: template Codim<0> :: Entity Element;
  SimpleLoadBalanceHandle ( const Grid &grid )
  : angle_( 0 )
  , maxRank_( grid.comm().size() )
  {}

  // this method is called before invoking the repartition method on the
  // grid, to check if the user defined partitioning needs to be readjusted
  bool repartition () 
  { 
    angle_ += 2.*M_PI/50.;
    return true;
  }

  /** This is the method, called from the grid for each macro element. 
      It returns the rank to which the element is to be moved. */
  int operator()( const Element &element ) const 
  { 
    typedef typename Element::Geometry::GlobalCoordinate Coordinate;
    Coordinate w = element.geometry().center();
    w -= Coordinate(0.5);
    if (w[0]*w[0]+w[1]*w[1] > 0.1 && maxRank_>0)
    { // distribute everything away from the center in equal slices
      double phi=arg(std::complex<double>(w[0],w[1]));
      if (w[1]<0) phi+=2.*M_PI;
      phi += angle_;
      phi *= double(maxRank_-1)/(2.*M_PI);
      int p = int(phi) % (maxRank_-1);
      return p+1;
    }
    else // keep the center on proc 0
      return 0;
  }
  // This method can simply return false, in which case ALUGrid will
  // internally compute the required information through some global
  // communication. To avoid this overhead the user can provide the ranks
  // of particians from which elements will be moved to the calling partitian.
  bool importRanks( std::set<int> &ranks) const { return false; }
private:
  double angle_;
  int maxRank_;
};

/********************************************************************
 *
 *  Simple weights used with ALUGrid load balancing
 *
 ********************************************************************/
template< class Grid >
struct SimpleLoadBalanceWeights
{
  typedef typename Grid :: Traits :: template Codim<0> :: Entity Element;
  SimpleLoadBalanceWeights ( const Grid &grid )
    : grid_( grid )
  {}

  /** This method is called for each macro element to determine the weight 
      in the dual graph. */
  long int operator()( const Element &element ) const 
  { 
    // return local id as weight for element 
    return grid_.localIdSet().id( element );
  }

protected:
  const Grid& grid_;
};

#endif // #ifndef LOADBALNCE_HH
