#ifndef ALUGRID_COMPATIBILITY_CHECK_HH_INCLUDED
#define ALUGRID_COMPATIBILITY_CHECK_HH_INCLUDED

#include <iostream>
#include <array>
#include <list>
#include <vector>
#include <algorithm>
#include <cassert>
#include <random>
#include <map>
#include <unordered_map>

#include <dune/common/hash.hh>
#include <dune/common/timer.hh>

struct BisectionCompatibilityParameters
{
  static int& variant ()
  {
    static int var = 0; // default is every vertex in set V0
    return var;
  }

  static int& threshold ()
  {
    static int var = 2; // default is 2 for threshold
    return var;
  }

  static int& useAnnouncedEdge ()
  {
    static int var = 1 ;  // default is use announced edges
    return var;
  }
};

//Class to correct the element orientation to make bisection work in 3d
// It provides different algorithms to orientate a grid.
// Also implements checks for compatibility.
template <class VertexVector, class PeriodicBoundaryVector>
class BisectionCompatibility
{
  typedef BisectionCompatibility< VertexVector, PeriodicBoundaryVector > ThisType;
public:
  // type of vertex coordinates stored inside the factory
  typedef VertexVector  VertexVectorType;

  typedef std::array<unsigned int, 3> FaceType;
  typedef std::vector< unsigned int > ElementType;
  typedef std::array<unsigned int, 2> EdgeType;
  typedef std::unordered_map< FaceType, EdgeType > FaceMapType;
  typedef std::pair< FaceType, EdgeType > FaceElementType;

protected:
  const VertexVectorType& vertices_;

  //the elements to be renumbered
  std::vector<ElementType> elements_;
  //the neighbouring structure
  FaceMapType neighbours_;
  // the number of vertices
  const size_t nVertices_;
  //A tag vector for vertices to
  //decide whether they are in V_1 or v_0
  std::vector<bool> containedInV0_;
  //the element types
  std::vector<int> types_;
  //possible periodic vertex identifications
  std::unordered_map<unsigned int, std::set<unsigned int> > periodicVertices_;

  //the 2 nodes of the refinement edge
  EdgeType type0nodes_;  // = stevensonRefinement_ ? 0,3 : 0,1 ;
  //the faces opposite of type 0 nodes
  EdgeType type0faces_;
  //The interior node of a type 1 element
  unsigned int type1node_;  // = stevensonRefinement_ ? 1 : 2;

  // 0 = put all vertices in V0,
  // 1 = longest edge,
  // 2 = least adjacent elements,
  // 3 = random ordering
  const int variant_   = BisectionCompatibilityParameters::variant() ;
  const int threshold_ = BisectionCompatibilityParameters::threshold() ;
  const bool useAnnouncedEdge_ = BisectionCompatibilityParameters::useAnnouncedEdge();

public:
  //constructor taking elements
  //assumes standard orientation elemIndex % 2
  BisectionCompatibility( const VertexVectorType& vertices,
                          const std::vector<ElementType>& elements,
                          const PeriodicBoundaryVector & perBoundaries)
    : vertices_( vertices ),
      elements_( elements ),
      nVertices_( vertices_.size() ),
      containedInV0_(nVertices_,true),
      types_(elements_.size(), 0),
      type0nodes_( EdgeType{0,3} ),
      type0faces_( EdgeType{3,0} ),
      type1node_( 1 )
  {
    //build the information about neighbours
    Dune::Timer timer;
    buildNeighbors();
#ifndef NDEBUG
    std::cout << "Build neighbors took " << timer.elapsed() << " sec." << std::endl;
#endif
    buildPeriodicVertices(perBoundaries);
  }

  //check for strong compatibility
  int stronglyCompatibleFaces ()
  {
    int result = 0;
    bool verbose = false;
    unsigned int bndFaces = 0;
    std::vector<int> nonCompatFacesAtVertex(nVertices_, 0 );
    for(auto&& face : neighbours_)
    {
      if( face.second[0] == face.second[1] )
      {
        bndFaces++;
      }
      else if(! checkStrongCompatibility(face, verbose))
      {
        ++result;
        for(size_t i =0; i < 3; ++i)
        {
          ++(nonCompatFacesAtVertex[face.first[i]]);
        }
      }
    }
#ifndef NDEBUG
    std::cout << "NotStrongCompatibleMacroFaces"  << " InnerFaces "  << " TotalFaces " << "Maximum/Vertex " << " Minimum/Vertex "  << std::endl;
    std::cout << result << " " << neighbours_.size() - bndFaces << " " << neighbours_.size() << " " <<  *(std::max_element(nonCompatFacesAtVertex.begin(), nonCompatFacesAtVertex.end())) << " " << *(std::min_element(nonCompatFacesAtVertex.begin(), nonCompatFacesAtVertex.end())) << std::endl << std::endl;
#endif
    return result;
  }

  //check grid for compatibility
  //i.e. walk over all faces and check their compatibility.
  bool compatibilityCheck ()
  {
    for(auto&& face : neighbours_)
    {
      if(!checkFaceCompatibility(face)) return false;
    }
    return true;
  }

  //print the neighbouring structure
  void printNB()
  {
    for(auto&& face : neighbours_)
    {
      std::cout << face.first[0] << "," << face.first[1] << "," << face.first[2] << " : " << face.second[0] << ", " << face.second[1] << std::endl;
    }
  }

  //print an element with orientation and all refinement edges
  void printElement(int index)
  {
    ElementType el = elements_[index];
    EdgeType edge;
    std::cout << "[" << el[0] ;
    for(int i=1; i < 4; ++i)
      std::cout << ","<< el[i] ;
    std::cout << "]  Refinement Edges: ";
    for(int i=0; i< 4; ++i)
    {
      getRefinementEdge(el, i, edge, types_[index]);
      std::cout << "[" << edge[0] << "," << edge[1] << "] ";
    }
    std::cout << " Type: " << types_[ index ] << " V1: ";
    for (size_t i = 0; i < 4; ++i) {
      if( ! containedInV0_[ el [ i ] ] )
        std::cout << el[i] << "," ;
    }
    std::cout <<  std::endl;
  }

  //an algorithm using only elements of type 0
  //it works by sorting the vertices in a global ordering
  //and tries to make as many reflected neighbors as possible.
  //
  // \param vector containing a double, by which vertices will be sorted
  //      if it is empty, weights will be created on the fly
  //
  bool type0Algorithm( std::vector<double> vertexWeights  )
  {
    //calculate the sets V0 and V1
    calculateV0( variant_, threshold_ );
    bool constructOrder = false;
    if( vertexWeights.empty() )
    {
      constructOrder = true;
      vertexWeights.resize( nVertices_, -1.0 );
    }
    else
    {
      //guarantee the same weight and inv0 for periodically identified vertices
      // may not work in some rare cases
      for( auto vtx : periodicVertices_ )
      {
        double weight = vertexWeights[vtx.first];
        bool inV0 = containedInV0_[vtx.first];
        for( unsigned identvtx : vtx.second )
        {
          vertexWeights[identvtx] = weight;
          containedInV0_[identvtx] = inV0;
        }
      }
      assert(vertexWeights.size() == nVertices_);
    }

    const bool useAnnounced = useAnnouncedEdge_;

    // all elements are type 0
    std::fill( types_.begin(), types_.end(), 0 );

    std::list<std::pair<FaceType, EdgeType> > activeFaceList; // use std::find to find
    std::vector<bool> doneElements(elements_.size(), false);

    // vector containing a double, by which vertices will be sorted, an index to the previous
    // and the the following vertex
    // -1 and -2 are the two ends (-1 before beginning, -2 after the end)
    std::vector< std::pair< int,int > > vertexOrder( nVertices_ , std::make_pair(-1,-2) );
    //if the difference of two doubles drops below eps, a rescale is done
    const double eps = std::numeric_limits< double >::epsilon() * double(nVertices_) * 10.0;


    // Now we walk over the elements`
    const unsigned int l1 = -1;
    const int numberOfElements = elements_.size();
    Dune::Timer timer;
    for(int counter = 0; counter < numberOfElements ; ++counter)
    {
      FaceElementType faceElement = std::make_pair( FaceType{l1,l1,l1}, EdgeType{l1,l1} );
      if(counter == 0 )
      {
          FaceType face;
          const ElementType& el0 = elements_[0];
          getFace(el0, type0faces_[0], face);
          faceElement = FaceElementType(std::make_pair( face , EdgeType{0,0} ) );

        if( constructOrder )
        {
          //orientate E_0 - add vertices to vertexOrder
          for(unsigned int i=0 ; i < 4 ; ++i)
          {
            int vtx = el0[ i ];
            vertexWeights[ vtx ] = i+1;
            // previous vertex index or -1
            vertexOrder[ vtx ].first  = i > 0 ? el0[ i-1 ] : -1;
            vertexOrder[ vtx ].second = i < 3 ? el0[ i+1 ] : -2;
          }
        }
      }
      else
      {
        assert( ! activeFaceList.empty() );
        //take element at first face from activeFaceList
        auto it = activeFaceList.begin();
        int el1 = it->second[ 0 ];
        int el2 = it->second[ 1 ];

        while( doneElements[ el1 ] && doneElements[ el2 ] )
        {
          activeFaceList.erase( it++ );
          assert( it != activeFaceList.end() );
          el1 = it->second[ 0 ];
          el2 = it->second[ 1 ];
        }

        faceElement = *it;
      }

      //el has been worked on
      int elIndex = faceElement.second[0];

      //neigh is to be worked on
      int neighIndex = faceElement.second[1];
      if(!doneElements[elIndex])
      {
        assert(doneElements[neighIndex] || counter == 0 );
        std::swap(elIndex, neighIndex);
      }

      assert(!doneElements[neighIndex] || counter == 0);
      doneElements[neighIndex] = true;
      ElementType el = elements_[elIndex];
      ElementType & neigh = elements_[neighIndex];
      unsigned int faceInEl = getFaceIndex(el, faceElement.first);
      unsigned int nodeInEl = 3 - faceInEl;
      unsigned int faceInNeigh = getFaceIndex(neigh, faceElement.first);
      unsigned int nodeInNeigh = 3 - faceInNeigh;

      //helper element that contains neigh
      // in the ordering of vertexPriorityList
      ElementType newNeigh(el);

      //insertion of new vertex before nodeInEl
      //i.e. < 0 means the vertex has not been worked on
      if( constructOrder &&  vertexWeights[ neigh [ nodeInNeigh ] ] < 0 )
      {
        int vxIndex = el[ nodeInEl ];

        //this takes care that the children will be reflected neighbors
        //if nodeInNeigh = 3 && nodeInEl = 0 insert after el[3]
        if( useAnnounced && (nodeInEl == type0nodes_[0] && nodeInNeigh == type0nodes_[1] ) )
        {
          double vxWeight = vertexWeights[ el[ nodeInNeigh] ];
          auto& vxPair = vertexOrder[ el[ nodeInNeigh ] ];
          // got to next vertex
          vxIndex = vxPair.second;
          if( vxIndex == -2 )
          {
            vertexWeights[ neigh [ nodeInNeigh ] ] = vxWeight + 1.0;
            vertexOrder[ neigh [ nodeInNeigh ] ] = std::make_pair( el[ nodeInNeigh ], -2 );
            vxPair.second = neigh [ nodeInNeigh ];
          }
        }

        if( vxIndex >= 0 )
        {
          //if nodeInNeigh = 0 && nodeInEl = 3 insert before el[0]
          if ( useAnnounced && ( nodeInEl == type0nodes_[1] && nodeInNeigh == type0nodes_[0] ) )
          {
            vxIndex = el[ nodeInNeigh ];
          }

          double vxWeight = vertexWeights[ vxIndex ];
          auto& vxPair = vertexOrder[ vxIndex ];
          assert( vxWeight >= 0 );

          // new vertex weight is average between previous and this one
          const int prevIdx = vxPair.first ;
          double prevOrder = ( prevIdx == -1 ) ? 0.0 : vertexWeights[ prevIdx ];
          double newOrder = 0.5 * (vxWeight + prevOrder);
          assert( vxWeight > newOrder );

          vertexWeights[ neigh [ nodeInNeigh ] ] =  newOrder ;
          vertexOrder[ neigh [ nodeInNeigh ] ] =  std::make_pair( prevIdx, vxIndex ) ;
          if( prevIdx >= 0 )
            vertexOrder[ prevIdx ].second = neigh [ nodeInNeigh ];
          vxPair.first = neigh [ nodeInNeigh ];

          if( (vxWeight - newOrder) < eps )
          {
#ifndef NDEBUG
            Dune::Timer restimer;
            std::cout << "Rescale vertex order weights." << std::endl;
#endif
            // We use a map to sort
            std::map< double, int > newVertexOrder;
            const int size = vertexOrder.size();
            for( int j=0; j<size; ++j )
            {
              if( vertexWeights[ j ] > 0.0 )
              {
                newVertexOrder.insert( std::make_pair( vertexWeights[ j ], j ) );
              }
            }

            int count = 1;
            for( const auto& vx: newVertexOrder )
            {
              assert( vx.second >=0 && vx.second < size );
              vertexWeights[ vx.second ] = count++ ;
            }

            for( int j=0; j<size; ++j )
            {
              if( vertexWeights[ j ] > 0.0 && vertexOrder[ j ].first >= 0 )
              {
                if( vertexWeights[ j ] <= vertexWeights[ vertexOrder[ j ].first ] )
                  std::abort();
              }
            }
#ifndef NDEBUG
            std::cout << "Rescale done, time = " << restimer.elapsed() << std::endl;
#endif
          }
        }
      }

      {
        // sort vertices in ascending order
        // again use map for sorting purposes
        std::map< double, int > vx;
        for( int j=0; j<4; ++j )
        {
          if(constructOrder)
            assert( vertexWeights[ neigh[ j ] ] > 0 );
          vx.insert( std::make_pair( vertexWeights[ neigh[ j ] ], j ) );
        }

        int count = 0;
        for( const auto& vxItem : vx )
        {
          newNeigh[ count++ ] = neigh[ vxItem.second ] ;
        }
      }

      //adjust type of newNeigh
      unsigned int type = 0;
      bool contained[4];
      for(unsigned int i =0; i < 4; ++i)
      {
        contained[i] = containedInV0_[newNeigh[i]];
      }
      //if all are in V0 or all are V1
      //we do nothing, set type to 0 and keep the order
      if( ( contained[0] && contained[1] && contained[2] && contained[3] ) || ( !contained[0] && !contained[1] && !contained[2]  &&  !contained[3]  )  )
      {
        //do nothing
        ;
      }
      else
      {
        //We append the vertices in V1 after the ones in V0
        //to have a globally consistent ordering
        auto it = newNeigh.begin();
        for(unsigned int i = 0 ; i < 4; ++i)
        {
          if( ! contained[ i ] )
          {
            newNeigh.push_back(newNeigh[i]);
            ++type;
            it = newNeigh.erase(it);
          }
          else
          {
            ++it;
          }
        }
        alugrid_assert( newNeigh.size() == 4 );
      }
      types_[neighIndex] = type % 3;

      //reorientate neigh using the helper element newNeigh
      //we use swaps to be able to track the elementOrientation_
      for(unsigned int i =0 ; i < 3; ++i)
      {
        if( newNeigh[i] != neigh[i] )
        {
          auto neighIt = std::find(neigh.begin() + i,neigh.end(),newNeigh[i]);
          std::swap(*neighIt,neigh[i]);
        }
      }
      //add and remove faces from activeFaceList
      for(unsigned int i = 0; i < 4 ; ++i)
      {
        getFace(neigh, i, faceElement);
        //do nothing for boundary
        if(faceElement.second[0] == faceElement.second[1])
          continue;

        //if face does not contain ref Edge
        if(i != type0faces_[0] && i != type0faces_[1] )
          activeFaceList.push_back(faceElement);
        else
          activeFaceList.push_front(faceElement);
      }

      /*
#ifndef NDEBUG
      const int onePercent = numberOfElements / 100 ;
      if( onePercent > 0 && counter % onePercent == 0 )
      {
       // std::cout << "Done: element " <<  counter << " of " << numberOfElements << " time used = " << timer.elapsed() << std::endl;
       // timer.reset();
      }
#endif
*/
    }// end elements ?

    return compatibilityCheck();
  }



  void returnElements(std::vector<ElementType> & elements,
                      std::vector<int>& types )
  {
    elements = elements_;
    types = types_;
  }


private:

  //check face for compatibility
  bool checkFaceCompatibility(std::pair<FaceType, EdgeType> face, bool verbose = false)
  {
    EdgeType edge1,edge2;
    int elIndex = face.second[0];
    int neighIndex = face.second[1];
    if(elIndex != neighIndex)
    {
      getRefinementEdge(elements_[elIndex], face.first, edge1, types_[elIndex]);
      getRefinementEdge(elements_[neighIndex], face.first, edge2, types_[neighIndex]);
      if(edge1 != edge2)
      {
        if (verbose)
        {
         /* std::cerr << "Face: " << face.first[0] << ", " << face.first[1] << ", " << face.first[2]
          << " has refinement edge: " << edge1[0] << ", " << edge1[1] <<
          " from one side and: " << edge2[0] << ", " << edge2[1] <<
          " from the other." << std::endl; */
          printElement(elIndex);
          printElement(neighIndex);
        }
        return false;
      }
    }
    return true;
  }

  //check face for strong compatibility
  bool checkStrongCompatibility(std::pair<FaceType, EdgeType> face, bool verbose = false)
  {
    int elIndex = face.second[0];
    int neighIndex = face.second[1];
    bool result = false;
    //if we are not boundary
    if(elIndex != neighIndex)
    {
      int elType = types_[elIndex];
      int neighType = types_[neighIndex];
      unsigned int elFaceIndex = getFaceIndex(elements_[elIndex], face.first);
      unsigned int elNodeIndex = 3 - elFaceIndex;
      unsigned int neighFaceIndex = getFaceIndex(elements_[neighIndex], face.first);
      unsigned int neighNodeIndex = 3 - neighFaceIndex;
      if(elType == neighType)
      {
        //if the local face indices coincide, they are reflected neighbors
        // if the refinement edge is not contained in the shared face, we
        // have reflected neighbors of the children, if the face is in the same direction
        // and the other edge of the refinement edge is the missing one.
        if( elNodeIndex == neighNodeIndex ||
           (elNodeIndex == type0nodes_[0] && neighNodeIndex == type0nodes_[1]) ||
           (elNodeIndex == type0nodes_[1] && neighNodeIndex == type0nodes_[0]) )
        { result = true; }
      }
      else
      {
        if( elType % 3  == (neighType +1) %3 )
        {
          std::swap(elType, neighType);
          std::swap(elNodeIndex, neighNodeIndex);
          std::swap(elFaceIndex, neighFaceIndex);
          std::swap(elIndex, neighIndex);
        }
        switch (elType) {
          case 0:
            assert(neighType == 1);
            if( ( neighNodeIndex == type1node_ ) && ( elNodeIndex == type0nodes_[0] || elNodeIndex == type0nodes_[1] ) )
            { result = true; }
            break;
          case 1:
            assert(neighType == 2);
            if( ( neighNodeIndex == type1node_ ) && ( elNodeIndex == type0nodes_[0] || elNodeIndex == type0nodes_[1] ) )
            { result = true; }
            break;
          case 2:
            assert(neighType == 0);
            if( ( neighNodeIndex == type1node_ ) && ( elNodeIndex == type0nodes_[0] || elNodeIndex == type0nodes_[1] ) )
            { result = true; }
            break;
          default:
            std::cerr << "No other types than 0,1,2 implemented. Aborting" << std::endl;
            abort();
        }
      }
    }
    else //boundary faces
    {
      result =true;
    }
    if (verbose && result == false )
    {
     /* std::cerr << "Face: " << face.first[0] << ", " << face.first[1] << ", " << face.first[2]
      << " has refinement edge: " << edge1[0] << ", " << edge1[1] <<
      " from one side and: " << edge2[0] << ", " << edge2[1] <<
      " from the other." << std::endl; */
      printElement(elIndex);
      printElement(neighIndex);
    }
    return result;
  }

  void fixNode(ElementType& el, int node)
  {
    if(!(node == type1node_))
    {
      //swap the node at the right position
      std::swap(el[node],el[type1node_]);
      //also swap two other nodes to keep the volume positive
      //2 and 0 are never type1node_
      std::swap(el[(type1node_+1)%4],el[(type1node_+2)%4]);
    }
  }

  //The rotations that keep the type 1 node fixed
  void rotate(ElementType& el)
  {
    std::swap(el[(type1node_+1)%4],el[(type1node_+2)%4]);
    std::swap(el[(type1node_+1)%4],el[(type1node_+3)%4]);
  }

  //get the sorted face of an element to a corresponding index
  //the index coincides with the missing vertex
  void getFace(ElementType el, int faceIndex, FaceType & face )
  {
    switch(faceIndex)
    {
    case 3 :
      face = {el[1],el[2],el[3]};
      break;
    case 2 :
      face = {el[0],el[2],el[3]};
      break;
    case 1 :
      face = {el[0],el[1],el[3]};
      break;
    case 0 :
      face = {el[0],el[1],el[2]};
      break;
    default :
      std::cerr << "index " << faceIndex << " NOT IMPLEMENTED FOR TETRAHEDRONS" << std::endl;
      std::abort();
    }
    std::sort(face.begin(), face.end());
    return;
  }

  void getFace(ElementType el, int faceIndex, FaceElementType & faceElement)
  {
    FaceType face;
    getFace(el, faceIndex, face);
    faceElement = *neighbours_.find(face);
  }

  //get the sorted initial refinement edge of a face of an element
  //this has to be adjusted, when using stevensonRefinement
  //orientation switches indices 2 and 3 in the internal ordering
  void getRefinementEdge(ElementType el, int faceIndex, EdgeType & edge, int type )
  {
    if(type == 0)
    {
      switch(faceIndex)
      {
        case 0 :
          edge = {el[0],el[2]};
          break;
        case 1 :
          edge = {el[0],el[3]};
          break;
        case 2 :
          edge = {el[0],el[3]};
          break;
        case 3 :
          edge =  {el[1],el[3]};
          break;
        default :
          std::cerr << "index " << faceIndex << " NOT IMPLEMENTED FOR TETRAHEDRONS" << std::endl;
          std::abort();
      }
    }
    else if(type == 1 || type == 2)
    {
      switch(faceIndex)
      {
        case 0 :
          edge = {el[0],el[2]};
          break;
        case 1 :
          edge = {el[0],el[3]};
          break;
        case 2 :
          edge = {el[0],el[3]};
          break;
        case 3 :
          edge =  {el[2],el[3]};
          break;
        default :
          std::cerr << "index " << faceIndex << " NOT IMPLEMENTED FOR TETRAHEDRONS" << std::endl;
          std::abort();
      }
    }
    else
    {
      std::cerr << "no other types than 0, 1, 2 implemented." << std::endl;
      std::abort();
    }
    std::sort(edge.begin(),edge.end());
    return;
  }

  //get the sorted initial refinement edge of a face of an element
  void getRefinementEdge(ElementType el, FaceType face, EdgeType & edge, int type )
  {
    getRefinementEdge(el, getFaceIndex(el, face), edge, type);
    return;
  }

  //get the index of a face in an elements
  //this could be improved by exploiting that faces are sorted
  int getFaceIndex(ElementType el, FaceType face)
  {
    for(int i =0; i<4 ; ++i)
    {
      if(!( el[i] == face[0] || el[i] == face[1]  || el[i] == face[2] ) )
        return 3 - i ;
    }
    return -1;
  }

  //build the structure containing the neighbors
  //consists of a face and the two indices belonging to
  //the elements that share said face
  //boundary faces have two times the same index
  //this is executed in the constructor
  void buildNeighbors()
  {
    // clear existing structures
    neighbours_.clear();

    FaceType face;
    EdgeType indexPair;

    unsigned int index = 0;
    const auto nend = neighbours_.end();
    for(auto&& el : elements_)
    {
      for(int i = 0; i< 4; ++i)
      {
        getFace(el, i, face);
        auto faceInList = neighbours_.find(face);
        if(faceInList == nend)
        {
          indexPair = {index, index};
          neighbours_.insert(std::make_pair (face, indexPair ) );
        }
        else
        {
          assert(faceInList != neighbours_.end());
          faceInList->second[1] = index;
        }
      }
      ++index;
    }
  }

  void buildPeriodicVertices( const PeriodicBoundaryVector perBoundaries )
  {
    for( auto perBnd : perBoundaries )
    {
      ElementType face0(perBnd.first.first.begin(),perBnd.first.first.end());
      ElementType face1(perBnd.second.first.begin(), perBnd.second.first.end());
      for(unsigned i = 0 ; i < face0.size(); i++ )
      {
        unsigned vtx0 = face0[i];
        unsigned vtx1 = face1[i];
        auto it = periodicVertices_.find(vtx0);
        if( it != periodicVertices_.end())
        {
          it->second.insert(vtx1);
        }
        else
        {
          periodicVertices_.insert(std::make_pair( vtx0, std::set<unsigned>({vtx1}) ));
        }
        it = periodicVertices_.find(vtx1);
        if( it != periodicVertices_.end())
        {
          it->second.insert(vtx0);
        }
        else
        {
          periodicVertices_.insert(std::make_pair( vtx1, std::set<unsigned>({vtx0}) ));
        }
      }
    }
  }



  /*!
     \brief This method is supposed to calculate V0 and V1
     \param variante :
     1 means collect all longest edges into V0, rest in V1
     2 means vertices with #(adjacent elements ) < threshold are in V1
     3 means random vertices are in V1 (for test purposes)
     everyhting else means V0 = all vertices
  */
  void calculateV0(int variante, int threshold)
  {
    //For now, everything is in V0
    switch (variante) {
      case 1:
        {
          std::fill(containedInV0_.begin(),containedInV0_.end(), false);
          std::vector<int> numberOfAdjacentRefEdges(nVertices_, 0);
          //we assume that the edges have been sorted and
          //the refinement edge is, where it belongs
          for(auto&& el : elements_)
          {
            numberOfAdjacentRefEdges [ el [ type0nodes_[ 0 ] ] ] ++;
            numberOfAdjacentRefEdges [ el [ type0nodes_[ 1 ] ] ] ++;
          }
          for(unsigned int i = 0; i <nVertices_ ; ++i)
          {
            if(numberOfAdjacentRefEdges[ i ] >= threshold )
            {
              containedInV0_[ i ] = true;
            }
          }
        }
        break;
      case 2:
        {
          std::vector<int> numberOfAdjacentElements(nVertices_, 0);
          std::vector<bool> vertexOnBoundary(nVertices_, false);
          for(auto&& neigh : neighbours_)
          {
            //We want to treat boundary vertices differently
            if(neigh.second[1] == neigh.second[0])
            {
              for(unsigned int i =0; i < 3 ; ++i)
              {
                vertexOnBoundary[ neigh.first [ i ] ] =true;
              }
            }
          }
          //calculate numberOfAdjacentElements
          for(auto&& el : elements_)
          {
            for(unsigned int i =0; i < 4; ++i)
            {
              numberOfAdjacentElements[ el [ i ] ] ++;
            }
          }
          for(unsigned int i = 0; i <nVertices_ ; ++i)
          {
            double bound = vertexOnBoundary[ i ] ? threshold / 2. : threshold ;
            if(numberOfAdjacentElements[ i ] < bound )
            {
              containedInV0_[ i ] = false;
            }
          }

        }
        break;
      case 3:
        {
          std::default_random_engine generator;
          std::uniform_int_distribution<int> distribution(1,6);
          for(unsigned int i = 0; i < nVertices_; ++i)
          {
            int roll = distribution(generator);  // generates number in the range 1..6
            if(roll < 3)
            {
              containedInV0_[ i ] = false;
            }
          }
        }
        break;
      default: ;
    }
    int sizeOfV1 = 0;
    int sizeOfV0 = 0;
    for(unsigned int i =0 ; i < nVertices_; ++i)
    {
      if( containedInV0_ [ i ] )
      {
        ++sizeOfV0;
      }
      else
      {
        ++sizeOfV1;
      }
    }
#ifndef NDEBUG
    std::cout << "#V0 #V1" << std::endl   << sizeOfV0 << " " << sizeOfV1 << std::endl;
#endif
  }

}; //class bisectioncompatibility



#endif //ALUGRID_COMPATIBILITY_CHECK_HH_INCLUDED
